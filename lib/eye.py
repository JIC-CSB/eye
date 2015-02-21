"""Iamge viewer"""

import argparse

import pygame

def display_image(filename):
    pygame.init()

    w = 800
    h = 600

    screen = pygame.display.set_mode((w, h)) 

    img = pygame.image.load(filename)

    screen.blit(img, (0, 0))

    pygame.display.flip()

def main():
    parser = argparse.ArgumentParser(__doc__)
    parser.add_argument('filename', help='Filename to load.')
    args = parser.parse_args()

    #display_image(args.filename)
    print pygame.image.get_extended()

if __name__ == '__main__':
    main()

