import urllib.request, json
import time
from enum import Enum
with urllib.request.urlopen("http://api.openweathermap.org/data/2.5/weather?q=Paderborn&appid=d3854ae2f0e16209429220f480a775f0") as url:
    data = json.loads(url.read().decode())
    print(data)
celvin = (data["main"]["temp"])
celcius = celvin - 273.15
celcius = celcius * 10
celcius = round(celcius)
celcius = celcius / 10


class daynames(Enum):
    Mon = "Mon"
    Tue = "Din"
    Wed = "Mit"
    Thu = "Don"
    Fri = "Fri"
    Sat = "Sam"
    Sun = "Son"

timeinfo = time.strftime("%H:%M %d.%m.%Y %a")
date = time.strftime("%d.%m.%Y")
dayname = time.strftime("%a")
print(getattr(daynames,dayname).value)

print("In Paderborn ist es %s Uhr am %s. Die Temperatur beträgt %s °C" % (timeinfo, date, celcius))
