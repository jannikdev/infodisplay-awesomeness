import urllib.request, json
import time
from enum import Enum

import imaplib
import base64
import os
from os import path
import email
from email.header import decode_header, make_header

# News
with urllib.request.urlopen("https://newsapi.org/v2/top-headlines?country=de&category=technology&apiKey=0e302cb4656543ed90585a207ea3eb21") as url:
    newsdata = json.loads(url.read().decode())
    news = (newsdata["articles"][0]["title"])
    #print(news)
# Wetter und Zeit daten
with urllib.request.urlopen("http://api.openweathermap.org/data/2.5/weather?q=Paderborn&appid=d3854ae2f0e16209429220f480a775f0") as url:
    data = json.loads(url.read().decode())
    #print(data)
celvin = (data["main"]["temp"])
conditioninfo = (data["weather"][0]["main"])
icon = (data["weather"][0]["icon"])
celsius = celvin - 273.15
celsius = celsius * 10
celsius = round(celsius)
celsius = celsius / 10


class daynames(Enum):
    Mon = "Mo"
    Tue = "Di"
    Wed = "Mi"
    Thu = "Do"
    Fri = "Fr"
    Sat = "Sa"
    Sun = "So"

class conditions(Enum):
    Clear = "Klarer Himmel"
    Drizzle = "Nieseln"
    Rain = "Regen"
    Clouds = "Bewölkt"
    Thunderstorm = "Gewitter"
    Snow = "Schnee"
    Smoke = "Nebel"
    Haze = "Nebel"
    Fog = "Nebel"
    Sand = "Nebel"
    Dust = "Nebel"
    Ash = "Nebel"
    Squall = "Nebel"
    Tornado = "Nebel"

timeinfo = time.strftime("%H:%M")
date = time.strftime("%d.%m.%Y")
dayname = time.strftime("%a")
dayname = getattr(daynames, dayname).value
condition = getattr(conditions, conditioninfo).value
def get(p):
    f = p
    if path.isfile(f):
        with open(f) as f:
            return f.read()
    else:
        return None

email_user = get("emaildaten").split(":")[0]
email_pass = get("emaildaten").split(":")[1]

mail = imaplib.IMAP4_SSL("imap.gmail.com", 993)

mail.login(email_user, email_pass)

mail.select("inbox")

type, data = mail.search(None, '(UNSEEN)')
mail_ids = data[0]
id_list = mail_ids.split()

mailcounter = 0
slot = False
for num in data[0].split():
    mailcounter = mailcounter + 1
    typ, data = mail.fetch(num, '(BODY.PEEK[])')
    raw_email = data[0][1]# converts byte literal to string removing b''
    raw_email_string = raw_email.decode('utf-8')
    email_message = email.message_from_string(raw_email_string)
    for response_part in data:
        if isinstance(response_part, tuple):
            msg = email.message_from_string(response_part[1].decode('utf-8'))
            email_subject = msg['subject']
            email_from = msg['from']
            try:
                sender = email_from.split("<")[1].split(">")[0]
            except:
                sender = email_from
            if email_subject.startswith("=?"):
                email_subject = decode_header(email_subject)[0][0].decode("utf-8")
            if slot:
                slot2b = email_subject
                slot2s = sender
                slot = False
            else:
                slot1b = email_subject
                slot1s = sender
                slot = True
            #print("Email von %s: %s" % (sender, email_subject))
print("%s:%s\n%s:%s\n%s neue Emails empfangen" % (slot1s, slot1b, slot2s, slot2b, mailcounter))
print("Es ist %s Uhr am %s, den %s. Die aktuelle Temperatur beträgt %s °C und das Wetter ist %s" % (timeinfo, dayname, date, celsius, condition))
print("Hier die aktuellsten News: " + news)

#Diese Daten sind für die Email verfügbar:
#slot1b = Betreff der aktuellsten Email
#slot1s = Absender der aktuellsten Email
#slot2b = Betreff der zweit aktuellsten Email
#slot2s = Absender der zweit aktuellsten Email
#mailcounter = Anzahl ungelesener Mails

#News Daten:
#news = Neuste News aus Deutschland

#Diese Daten sind für das Wetter verfügbar:
#timeinfo = Aktuelle Uhrzeit in %H:%M
#dayname = Tageskürzel auf deutsch
#date = Datum
#celcius = Temperatur in Paderborn
#condition = Aktuelle Wetterlage
