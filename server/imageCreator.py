import imgkit
import platform
from PIL import Image
import time
from threading import Thread

def GetPng():
    options = {
        'format': 'png',
        'crop-h': '384',
        'crop-w': '640',
        'crop-x': '8',
        'crop-y': '8',
        'encoding': "ASCII",
    }
    if platform.system() == 'Windows':
        config = imgkit.config(wkhtmltoimage='C:/Program Files/wkhtmltopdf/bin/wkhtmltoimage.exe')
    elif platform.system() == 'Linux':
        config = imgkit.config(wkhtmltoimage='/usr/bin/wkhtmltoimage')

    pathToOutputPng = 'static/outputs/out.png'
    pathToOutputTxt = 'static/outputs/content.txt'
    imgkit.from_url('172.16.1.60:5000', pathToOutputPng, options=options, config=config)
    file = open(pathToOutputTxt, 'w')
    file.write("Lorem")
    img = Image.open(pathToOutputPng)
    isBlack = False
    pixelCounter = 0
    for y in range(384):
        for x in range(640):
            white = (255, 255, 255, 255)
            pixel = img.getpixel((x,y))
            if pixel == white:
                if isBlack:
                    file.write(str(pixelCounter) + ";")
                    isBlack = False
                    pixelCounter = 0
            else:
                if not isBlack:
                    file.write(str(pixelCounter) + ";")
                    isBlack = True
                    pixelCounter = 0
            pixelCounter += 1
    if pixelCounter != 0:
        file.write(str(pixelCounter) + ";")
    file.write("ipsum")
    file.close()

class worker(Thread):
    def run(self):
        running = True
        while running:
            print("starting to scrape and convert")
            GetPng()
            if open("./static/killImageCreator.txt", "r").read() != "kill":
                print("done scraping and converting. sleeping for 25 sec now")
                time.sleep(25)
            else:
                running = False

if __name__ == '__main__':
    worker().start()