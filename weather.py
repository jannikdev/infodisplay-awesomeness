import urllib.request, json

with urllib.request.urlopen("http://samples.openweathermap.org/data/2.5/weather?q=London,uk&appid=b6907d289e10d714a6e88b30761fae22") as url:
    data = json.loads(url.read().decode())
    #print(data)
celvin = (data["main"]["temp"])
celcius = celvin - 273.15
celcius = celcius * 10
celcius = round(celcius)
celcius = celcius / 10

print(celcius)

