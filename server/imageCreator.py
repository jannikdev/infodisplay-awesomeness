import imgkit
from PIL import Image

def GetPng():
    options = {
        'format': 'png',
        'crop-h': '384',
        'crop-w': '640',
        'crop-x': '8',
        'crop-y': '8',
        'encoding': "ASCII",
    }
    # config = imgkit.config(wkhtmltoimage='C:\\Program Files\\wkhtmltopdf\\bin\\wkhtmltoimage.exe')
    config = imgkit.config(wkhtmltoimage='/usr/bin/wkhtmltoimage')
    imgkit.from_url('172.16.1.60:5000', 'out.png', options=options, config=config)
    file = open(r"static/content.txt", "w")
    file.write("Lorem")
    img = Image.open("out.png")
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
    file.close()
GetPng()