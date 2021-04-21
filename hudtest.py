import serial
from time import sleep

import pygame
from pygame.locals import *

pygame.init()
 
bluetoothSerial = serial.Serial( "/dev/rfcomm0", baudrate=9600 )

screen = pygame.display.set_mode((0,0),pygame.FULLSCREEN)

screen_w = screen.get_width()
screen_h = screen.get_height()

circle1_y = screen_h * .25
circle2_y = screen_h * .25
circle3_y = screen_h * .25

circle4_y = screen_h * .75
circle5_y = screen_h * .75
circle6_y = screen_h * .75

circle1_x = screen_w * .25
circle4_x = screen_w * .25

circle2_x = screen_w * .5
circle5_x = screen_w * .5

circle3_x = screen_w * .75
circle6_x = screen_w * .75

circle_rad = (circle2_x - circle1_x)/2

speed_text_x = screen_w * .25
speed_text_y = screen_h * .15

rpm_text_x = screen_w * .5
rpm_text_y = screen_h * .15

coolanttemp_text_x = screen_w * .19
coolanttemp_text_y = screen_h * .65

oiltemp_text_x = screen_w * .47
oiltemp_text_y = screen_h * .65

airtemp_text_x = screen_w * .72
airtemp_text_y = screen_h * .65

fuellevel_text_x = screen_w * .71
fuellevel_text_y = screen_h * .15

rpm_display_x = screen_w * .25
rpm_display_y = screen_h * .25

speed_display_x = screen_w * .5
speed_display_y = screen_h * .25

coolant_display_x = screen_w * .25
coolant_display_y = screen_h * .75

oil_display_x = screen_w * .50
oil_display_y = screen_h * .75

air_display_x = screen_w * .75
air_display_y = screen_h * .75

fuel_display_x = screen_w * .75
fuel_display_y = screen_h * .25

headerFont = pygame.font.SysFont("Arial", 50)
digitFont = pygame.font.SysFont("Arial", 50)

white = (255,255,255)
black = (0,0,0)
grey = (112,128,144)
red = (255,0,0)

speed = 0
rpm = 0
fuellevel = 0
coolanttemp = 0
oiltemp = 0
airtemp = 0

def draw_hud():
	screen.fill(grey)
	pygame.draw.circle(screen, black, (int(circle1_x), int(circle1_y)), int(circle_rad), 5)
	pygame.draw.circle(screen, black, (int(circle2_x), int(circle2_y)), int(circle_rad), 5)
	pygame.draw.circle(screen, black, (int(circle3_x), int(circle3_y)), int(circle_rad), 5)
	pygame.draw.circle(screen, black, (int(circle4_x), int(circle4_y)), int(circle_rad), 5)
	pygame.draw.circle(screen, black, (int(circle5_x), int(circle5_y)), int(circle_rad), 5)
	pygame.draw.circle(screen, black, (int(circle6_x), int(circle6_y)), int(circle_rad), 5)

	speed_text = headerFont.render("SPEED", True, black)
	rpm_text = headerFont.render("RPM", True, black)
	oiltemp_text = headerFont.render("OIL TEMP", True, black)
	airtemp_text = headerFont.render("AIR TEMP", True, black)
	coolanttemp_text = headerFont.render("COOLANT TEMP", True, black)
	fuellevel_text = headerFont.render("FUEL LEVEL", True, black)

	speed_text_loc = speed_text.get_rect(center=(speed_text_x, speed_text_y))
	rpm_text_loc = speed_text.get_rect(center=(rpm_text_x, rpm_text_y))
	coolanttemp_text_loc = speed_text.get_rect(center=(coolanttemp_text_x, coolanttemp_text_y))
	oiltemp_text_loc = speed_text.get_rect(center=(oiltemp_text_x, oiltemp_text_y))
	airtemp_text_loc = speed_text.get_rect(center=(airtemp_text_x, airtemp_text_y))
	fuellevel_text_loc = speed_text.get_rect(center=(fuellevel_text_x, fuellevel_text_y))

	screen.blit(speed_text, speed_text_loc)
	screen.blit(rpm_text, rpm_text_loc)
	screen.blit(oiltemp_text, oiltemp_text_loc)
	screen.blit(airtemp_text, airtemp_text_loc)
	screen.blit(coolanttemp_text, coolanttemp_text_loc)
	screen.blit(fuellevel_text, fuellevel_text_loc)

running = True

while running:
	for event in pygame.event.get():
		if event.type == KEYDOWN:
			if event.key == K_ESCAPE:
				running = False
			elif event.type == QUIT:
				running = False

	draw_hud()


	infromserial = bluetoothSerial.readline()
	brokenup = infromserial.split()

	while len(brokenup) < 5:
		infromserial = bluetoothSerial.readline()
		brokenup = infromserial.split()		

	rpmDisplay = digitFont.render(brokenup[0], 3, white)
	speedDisplay = digitFont.render(brokenup[1], 3, white)
	coolantDisplay = digitFont.render(brokenup[2], 3, white)
	oilDisplay = digitFont.render(brokenup[3], 3, white)
	airDisplay = digitFont.render(brokenup[4], 3, white)
	fuelDisplay = digitFont.render(brokenup[5] + "%", 3, white)

	screen.blit(rpmDisplay, (rpm_display_x , rpm_display_y)) 
	screen.blit(speedDisplay, (speed_display_x , speed_display_y)) 
	screen.blit(coolantDisplay, (coolant_display_x , coolant_display_y)) 
	screen.blit(oilDisplay, (oil_display_x , oil_display_y)) 
	screen.blit(airDisplay, (air_display_x , air_display_y)) 
	screen.blit(fuelDisplay, (fuel_display_x , fuel_display_y)) 

	pygame.display.update()
	pygame.display.flip()



	

