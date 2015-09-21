"""Simple viewer for images in a director."""
import os
import os.path
import argparse
import re

import sys
import ctypes
from sdl2 import *
from sdl2.sdlimage import IMG_LoadTexture
import sdl2.ext

def sorted_nicely(l):
    convert = lambda text: int(text) if text.isdigit() else text
    alphanum_key = lambda key: [ convert(c) for c in re.split('([0-9]+)', key) ]
    return sorted(l, key = alphanum_key)


class ImageContainer(list):
    def __init__(self):
        """Initialise the image file path container."""
        self._current_id = 0

    def current(self):
        """Return the current image fpath."""
        return self[self._current_id]

    def next(self):
        """Update the current image fpath to the next one."""
        self._current_id += 1
        if self._current_id >= len(self):
            self._current_id = 0

    def prev(self):
        """Update the current image fpath to the previous one."""
        self._current_id -= 1
        if self._current_id < 0:
            self._current_id = len(self) - 1
        
    def load_images(self, directory, prefix=None):
        """Load images file paths from a directory."""
        for fpath in sorted_nicely(os.listdir(directory)):
            if prefix is not None and not fpath.startswith(prefix):
                # If a prefix has been defined make sure that only
                # those files with that prefix are considered.
                continue
            if fpath.endswith(".png"):
                self.append(os.path.join(directory, fpath))

class View(object):
    def __init__(self):
        self._zoom_level = 0
        self._sizes = [(500, 500), (400, 400), (300, 300), (100, 100)]
        self._x = 0
        self._y = 0

    def current(self):
        """Return the current location and zoom."""
        return self._x, self._y, self._sizes[self._zoom_level]

    def _zoom_center(self, org_size, new_size):
        """Center view after zoom."""
        org_w, org_h = org_size
        new_w, new_h = new_size
        offset_w = (org_w - new_w) // 2
        offset_h = (org_h - new_h) // 2
        self._x = self._x + offset_w
        self._y = self._y + offset_h

    def zoom_in(self):
        """Zoom in on center of view."""
        org_size = self._sizes[self._zoom_level]
        self._zoom_level += 1
        if self._zoom_level >= len(self._sizes):
            self._zoom_level = len(self._sizes) - 1
        new_size = self._sizes[self._zoom_level]
        self._zoom_center(org_size, new_size)

    def zoom_out(self):
        """Zoom out in on center of view."""
        org_size = self._sizes[self._zoom_level]
        self._zoom_level -= 1
        if self._zoom_level < 0:
            self._zoom_level = 0
        new_size = self._sizes[self._zoom_level]
        self._zoom_center(org_size, new_size)
        
    def move_left(self, step=10):
        """Shift view some steps to the left."""
        self._x -= step
        if self._x < 0:
            self._x = 0


    def move_right(self, step=10):
        """Shift view some steps to the right."""
        self._x += step
        zoom_width = self._sizes[self._zoom_level][0]
        move_span = 500 - zoom_width
        if self._x > move_span:
            self._x = move_span

    def move_up(self, step=10):
        """Shift view some steps up."""
        self._y -= step
        if self._y < 0:
            self._y = 0

    def move_down(self, step=10):
        """Shift view some steps down."""
        self._y += step
        zoom_height = self._sizes[self._zoom_level][1]
        move_span = 500 - zoom_height
        if self._y > move_span:
            self._y = move_span

class Viewer(object):
    """Basic viewer."""

    def __init__(self, images):
        self._images = images
        self._view = View()
        SDL_Init(SDL_INIT_VIDEO)
        self.window = SDL_CreateWindow(b"Image Viewer",
                              SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              500, 500, SDL_WINDOW_SHOWN)

        self.renderer = SDL_CreateRenderer(self.window, -1, 0)
        self.display_rect = SDL_Rect(0, 0, 500, 500)
        self.zoom_rect = SDL_Rect(0, 0, 500, 500)
        self.update_image()
        self.run()

    def update_zoom(self):
        """Update the zoom box."""
        x, y, (w, h) = self._view.current()
        self.zoom_rect = SDL_Rect(x, y, w, h)
        
    def update_image(self):
        """Display the next image and update the window title."""
        self.update_zoom()
        fpath = self._images.current()
        SDL_SetWindowTitle(self.window, b"Image Viewer: {}".format(os.path.basename(fpath)))
        texture = IMG_LoadTexture(self.renderer, fpath)
        SDL_RenderClear(self.renderer)
        SDL_RenderCopy(self.renderer, texture, self.zoom_rect, self.display_rect)
        SDL_RenderPresent(self.renderer)

    def next(self):
        """Display the next image."""
        self._images.next()
        self.update_image()
    
    def prev(self):
        """Display the prvious image."""
        self._images.prev()
        self.update_image()
        
    def zoom_in(self):
        """Zoom in on the image."""
        self._view.zoom_in()
        self.update_image()

    def zoom_out(self):
        """Zoom out from the image."""
        self._view.zoom_out()
        self.update_image()

    def move_left(self):
        """Shift the view to the left."""
        self._view.move_left()
        self.update_image()

    def move_right(self):
        """Shift the view to the right."""
        self._view.move_right()
        self.update_image()

    def move_up(self):
        """Shift the view up."""
        self._view.move_up()
        self.update_image()

    def move_down(self):
        """Shift the view down."""
        self._view.move_down()
        self.update_image()

    def run(self):
        """Run the application."""
        running = True
        event = SDL_Event()
        while running:
            while SDL_PollEvent(ctypes.byref(event)) != 0:
                if event.type == SDL_QUIT:
                    running = False
                    break
                if event.type == SDL_KEYUP:
                    if event.key.keysym.sym == sdl2.SDLK_RIGHT:
                        self.next()
                    if event.key.keysym.sym == sdl2.SDLK_LEFT:
                        self.prev()
                    if event.key.keysym.sym == sdl2.SDLK_UP:
                        self.zoom_in()
                    if event.key.keysym.sym == sdl2.SDLK_DOWN:
                        self.zoom_out()
                    if event.key.keysym.sym == sdl2.SDLK_l:
                        self.move_right()
                    if event.key.keysym.sym == sdl2.SDLK_h:
                        self.move_left()
                    if event.key.keysym.sym == sdl2.SDLK_k:
                        self.move_up()
                    if event.key.keysym.sym == sdl2.SDLK_j:
                        self.move_down()
                if event.type == SDL_MOUSEBUTTONDOWN:
                    if event.button.button == SDL_BUTTON_LEFT:
                        print("x: {}, y: {}".format(event.button.x,
                            event.button.y))

        SDL_DestroyWindow(window)
        SDL_Quit()
        return 0

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("input_dir", help="Input directory")
    parser.add_argument("-p", "--prefix", default=None, help="File name prefix")

    args = parser.parse_args()

    images = ImageContainer()
    images.load_images(args.input_dir, args.prefix)

    viewer = Viewer(images)

    sys.exit(viewer.run())
