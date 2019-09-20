import urllib.request, json
import time
with urllib.request.urlopen("http://api.openweathermap.org/data/2.5/weather?q=Paderborn&appid=d3854ae2f0e16209429220f480a775f0") as url:
    data = json.loads(url.read().decode())
    #print(data)
celvin = (data["main"]["temp"])
celcius = celvin - 273.15
celcius = celcius * 10
celcius = round(celcius)
celcius = celcius / 10

time = time.strftime("%H:%M %d.%m.%Y")
date = time.__str__()[6:]
time = time.__str__()[:-11]

print("In Paderborn ist es %s Uhr am %s. Die Temperatur beträgt %s °C" % (time, date, celcius))
