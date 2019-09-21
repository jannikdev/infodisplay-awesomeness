import urllib.request, json

def GetTemperature():
    link = "http://api.openweathermap.org/data/2.5/weather?q=Paderborn&appid=d3854ae2f0e16209429220f480a775f0"
    with urllib.request.urlopen(link) as url:
        data = json.loads(url.read().decode())
    celvin = (data["main"]["temp"])
    celsius = celvin - 273.15
    celsius = celsius * 10
    celsius = round(celsius)
    celsius = celsius / 10
    return celsius
