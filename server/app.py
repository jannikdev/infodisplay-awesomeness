from flask import Flask, render_template
import datetime

#-------------------------------------------------------------------------------------------------------
import urllib.request, json
link = "http://api.openweathermap.org/data/2.5/weather?q=Paderborn&appid=d3854ae2f0e16209429220f480a775f0"
with urllib.request.urlopen(link) as url:
    data = json.loads(url.read().decode())
    #print(data)
celvin = (data["main"]["temp"])
celcius = celvin - 273.15
celcius = celcius * 10
celcius = round(celcius)
celcius = celcius / 10

#print(celcius)
#-------------------------------------------------------------------------------------------------------

app = Flask(__name__, static_url_path='/static')

@app.route('/')

def hello():
    now = datetime.datetime.now()
    timeString = now.strftime("%Y-%m-%d %H:%M")
    templateData = {
        'title' : 'HELLO!',
        'time': timeString
    }
    return render_template('index.html', **templateData)

if __name__ == '__main__':
    app.run(debug=True, host='0.0.0.0')