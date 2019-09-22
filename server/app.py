from flask import Flask, render_template
from server.weather import GetTemperature, GetEmails, GetNews, GetCondition, GetWeatherIcon, GetTime, GetDate, GetDayname

app = Flask(__name__, static_url_path='/static')

@app.route('/')

def hello():
    templateData = {
        'title' : 'Infodisplay',
        'time': str(GetTime()),
        'celsius': str(GetTemperature()) + "°C",
        'condition' : str(GetCondition()),
        'icon' : str(GetWeatherIcon()),
        'date' : str(GetDate()),
        'dayname' : str(GetDayname())
    }
    return render_template('index.html', **templateData)

if __name__ == '__main__':
    app.run(debug=True, host='0.0.0.0')
