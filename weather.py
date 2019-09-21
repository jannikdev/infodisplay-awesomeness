import urllib.request, json
import time
from enum import Enum

import imaplib
import base64
import os
import email
from email.header import decode_header, make_header



# Wetter und Zeit daten
with urllib.request.urlopen("http://api.openweathermap.org/data/2.5/weather?q=Paderborn&appid=d3854ae2f0e16209429220f480a775f0") as url:
    data = json.loads(url.read().decode())
    print(data)
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

#infodisplaymail@gmail.com
#Pad3rb0rn
email_user = "infodisplaymail@gmail.com"
email_pass = "Pad3rb0rn"

mail = imaplib.IMAP4_SSL("imap.gmail.com", 993)

mail.login(email_user, email_pass)

mail.select("inbox")

type, data = mail.search(None, 'ALL')
mail_ids = data[0]
id_list = mail_ids.split()

for num in data[0].split():
    typ, data = mail.fetch(num, '(RFC822)' )
    raw_email = data[0][1]# converts byte literal to string removing b''
    raw_email_string = raw_email.decode('utf-8')
    email_message = email.message_from_string(raw_email_string)
    subject = email_message.__str__()[:].split("Subject: ")[1].split("To: ")[0].split("MIME-Version: ")[0]
    for response_part in data:
        if isinstance(response_part, tuple):
            msg = email.message_from_string(response_part[1].decode('utf-8'))
            email_subject = msg['subject']
            email_from = msg['from']
            sender = email_from.split("<")[1].split(">")[0]
            if not subject.startswith("=?"):
                print("Email von %s: %s" % (sender, subject))
