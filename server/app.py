from flask import Flask, render_template
from weather import GetTemperature
import datetime

app = Flask(__name__, static_url_path='/static')

@app.route('/')

def hello():
    now = datetime.datetime.now()
    timeString = now.strftime("%Y-%m-%d %H:%M")
    templateData = {
        'title' : 'Infodisplay',
        'time': timeString,
        'celsius': str(GetTemperature()) + "°C"
    }
    return render_template('index.html', **templateData)

if __name__ == '__main__':
    app.run(debug=True, host='0.0.0.0')