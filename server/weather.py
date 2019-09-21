import urllib.request, json
import time
from enum import Enum
import imaplib
import base64
from os import path
import os
import email
from email.header import decode_header, make_header

class daynames(Enum):
    Mon = "Montag"
    Tue = "Dienstag"
    Wed = "Mittwoch"
    Thu = "Donnerstag"
    Fri = "Freitag"
    Sat = "Samstag"
    Sun = "Sonntag"

class conditions(Enum):
    Clear = "Klar"
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

def GetWeatherApi():
    link = "http://api.openweathermap.org/data/2.5/weather?q=Paderborn&appid=d3854ae2f0e16209429220f480a775f0"
    with urllib.request.urlopen(link) as url:
        return json.loads(url.read().decode())

def GetNews():
    link = "https://newsapi.org/v2/top-headlines?country=de&category=technology&apiKey=0e302cb4656543ed90585a207ea3eb21"
    with urllib.request.urlopen(link) as url:
        newsdata = json.loads(url.read().decode())
        return newsdata["articles"][0]["title"]

def GetTemperature():
    data = GetWeatherApi()
    celsius = round((data["main"]["temp"] - 273.15) * 10) / 10
    return celsius

def GetWeatherCondition():
    data = GetWeatherApi()
    condition = data["weather"][0]["main"]
    return condition

def GetWeatherIcon():
    data = GetWeatherApi()
    icon = data["weather"][0]["icon"]
    return icon

def getTime():
    return time.strftime("%H:%M")

def getDate():
    return time.strftime("%d.%m.%Y")

def getDayname():
    dayname = time.strftime("%a")
    dayname = getattr(daynames, dayname).value
    return dayname

def getCondition():
    return getattr(conditions, GetWeatherCondition()).value

def GetEmails():
    def get():
        f = "emaildaten"
        if path.isfile(f):
            with open(f) as f:
                return f.read()
        else:
            return None

    email_user = get().split(":")[0]
    email_pass = get().split(":")[1]

    mail = imaplib.IMAP4_SSL("imap.gmail.com", 993)
    mail.login(email_user, email_pass)
    mail.select("inbox")

    type, data = mail.search(None, '(UNSEEN)')

    mailcounter = 0
    slot = False
    for num in data[0].split():
        mailcounter = mailcounter + 1
        typ, data = mail.fetch(num, '(BODY.PEEK[])')
        for response_part in data:
            if isinstance(response_part, tuple):
                msg = email.message_from_string(tuple(response_part)[1].decode('utf-8'))
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
    #print("%s:%s\n%s:%s\n%s neue Emails empfangen" % (slot1s, slot1b, slot2s, slot2b, mailcounter))
    return (slot1s, slot1b, slot2s, slot2b, mailcounter)



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
#celsius = Temperatur in Paderborn
#condition = Aktuelle Wetterlage
