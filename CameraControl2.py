import pygame
import asyncio, sys, os
from onvif import ONVIFCamera
import cv2
import numpy as np

IP = "192.168.0.100"  # Camera IP address
PORT = 2020  # Port
USER = "Fausto"  # Username
PASS = "123456"  # Password

moverequest = None
ptz = None
active = False

# camera = cv2.VideoCapture(0)
print(cv2.getBuildInformation())
camera = cv2.VideoCapture('rtsp://Fausto:123456@192.168.0.100:554/stream2', cv2.CAP_FFMPEG)
# camera = cv2.VideoCapture("rtspsrc location=rtsp://Fausto:123456@192.168.0.100:554/stream2 ! decodebin ! videoconvert ! video/x-raw,framerate=30/1 ! appsink drop=true sync=false", cv2.CAP_GSTREAMER)
camera.set(cv2.CAP_PROP_BUFFERSIZE, 1)

pygame.init()
pygame.display.set_caption("TP Link Tapo C200 Control")

SCREEN_WIDTH = 600
SCREEN_HEIGHT = 400

screen = pygame.display.set_mode((SCREEN_WIDTH, SCREEN_HEIGHT))

panning = 0
tilting = 0
speed = 1
cameraMovement = {"pan": panning, "tilt": tilting}
run = True
left = False
right = False
up = False
down = False


def do_move(ptz, request):
    # Start continuous move
    global active
    print("Do move... ", active)
    if active:
        print("Active")
        ptz.Stop({'ProfileToken': request.ProfileToken})
    active = True
    print("After active... ", active)
    if cameraMovement["pan"] or cameraMovement["tilt"] != 0:
        ptz.ContinuousMove(request)
        print(ptz.ContinuousMove(request))


def move_setup(ptz, request, pan, tilt):
    print('move...')
    request.Velocity.PanTilt.x = pan
    request.Velocity.PanTilt.y = tilt
    do_move(ptz, request)

def setup_move():
    mycam = ONVIFCamera(IP, PORT, USER, PASS)
    # Create media service object
    media = mycam.create_media_service()

    # Create ptz service object
    global ptz
    ptz = mycam.create_ptz_service()

    # Get target profile
    media_profile = media.GetProfiles()[0]

    # Get PTZ configuration options for getting continuous move range
    request = ptz.create_type('GetConfigurationOptions')
    request.ConfigurationToken = media_profile.PTZConfiguration.token
    ptz_configuration_options = ptz.GetConfigurationOptions(request)

    global moverequest
    moverequest = ptz.create_type('ContinuousMove')
    moverequest.ProfileToken = media_profile.token
    if moverequest.Velocity is None:
        moverequest.Velocity = ptz.GetStatus({'ProfileToken': media_profile.token}).Position
        moverequest.Velocity.PanTilt.space = ptz_configuration_options.Spaces.ContinuousPanTiltVelocitySpace[0].URI
        # moverequest.Velocity.Zoom.space = ptz_configuration_options.Spaces.ContinuousZoomVelocitySpace[0].URI

    # Get range of pan and tilt
    # NOTE: X and Y are velocity vector
    global XMAX, XMIN, YMAX, YMIN
    XMAX = ptz_configuration_options.Spaces.ContinuousPanTiltVelocitySpace[0].XRange.Max
    XMIN = ptz_configuration_options.Spaces.ContinuousPanTiltVelocitySpace[0].XRange.Min
    YMAX = ptz_configuration_options.Spaces.ContinuousPanTiltVelocitySpace[0].YRange.Max
    YMIN = ptz_configuration_options.Spaces.ContinuousPanTiltVelocitySpace[0].YRange.Min


if __name__ == '__main__':
    setup_move()
    while run:
        # Camera Movement Control
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                run = False
            if event.type == pygame.KEYDOWN:
                print(event)
                # Pan
                if event.key == pygame.K_a:
                    panning = -0.5
                    left = True
                if event.key == pygame.K_d:
                    panning = 0.5
                    right = True
                # Tilt
                if event.key == pygame.K_s:
                    tilting = -0.5
                    down = True
                if event.key == pygame.K_w:
                    tilting = 0.5
                    up = True
                # Speed
                if event.key == pygame.K_LSHIFT:
                    speed = 2
            if event.type == pygame.KEYUP:
                print(event)
                if event.key == pygame.K_a:
                    panning = 0
                    left = False
                if event.key == pygame.K_d:
                    panning = 0
                    right = False
                # Tilt
                if event.key == pygame.K_s:
                    tilting = 0
                    down = False
                if event.key == pygame.K_w:
                    tilting = 0
                    up = False
                # Speed
                if event.key == pygame.K_LSHIFT:
                    speed = 1
        if left and right is True:
            panning = 0
        else:
            cameraMovement["pan"] = panning*speed
        if up and down is True:
            tilting = 0
        else:
            cameraMovement["tilt"] = tilting*speed
        print(cameraMovement)
        move_setup(ptz, moverequest, cameraMovement["pan"], cameraMovement["tilt"])
        # Camera Image Display
        ret, frame = camera.read()
        screen.fill([0, 0, 0])
        frame = cv2.resize(frame, (SCREEN_WIDTH, SCREEN_HEIGHT), interpolation=cv2.INTER_LINEAR)
        frame = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
        frame = np.rot90(frame)
        frame = cv2.flip(frame, 0)
        frame = pygame.surfarray.make_surface(frame)
        screen.blit(frame, (0, 0))
        pygame.display.update()

pygame.quit()
