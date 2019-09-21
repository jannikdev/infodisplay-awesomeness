import urllib.request, json
import time
from enum import Enum



# Wetter und Zeit daten
with urllib.request.urlopen("http://api.openweathermap.org/data/2.5/weather?q=Paderborn&appid=d3854ae2f0e16209429220f480a775f0") as url:
    data = json.loads(url.read().decode())
    #print(data)
celvin = (data["main"]["temp"])
celcius = celvin - 273.15
celcius = celcius * 10
celcius = round(celcius)
celcius = celcius / 10


class daynames(Enum):
    Mon = "Mo"
    Tue = "Di"
    Wed = "Mi"
    Thu = "Do"
    Fri = "Fr"
    Sat = "Sa"
    Sun = "So"

timeinfo = time.strftime("%H:%M %d.%m.%Y %a")
date = time.strftime("%d.%m.%Y")
dayname = time.strftime("%a")
dayname = getattr(daynames,dayname).value



