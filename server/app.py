from flask import Flask, render_template
from server.weather import GetTemperature, GetEmails, GetNews, GetCondition, GetWeatherIcon, GetTime, GetDate, GetDayname

app = Flask(__name__, static_url_path='/static')

@app.route('/')

def hello():
    if int(GetEmails()[4]) > 0:
        bottom = '<div class="bottom">Sie haben 0 neue Nachrichten, deswegen die aktuellsten News: %s</div>' % str(GetNews())
        index = "index"
    elif int(GetEmails()[4]) == 1:
        bottom = '<div>%s neue Emails:</<div>\n<div>%s: %s</div>' % (str(GetEmails()[4]), str(GetEmails()[0]), str(GetEmails()[1]))
        index = "indexmail"
    else:
        bottom = '<div>%s neue Emails:</<div>\n<div>%s: %s</div>\n<div>%s: %s</<div>' % (str(GetEmails()[4]), str(GetEmails()[0]), str(GetEmails()[1]), str(GetEmails()[2]), str(GetEmails()[3]))
        index = "indexmail"
    templateData = {
        'index' : index,
        'bottom' : bottom,
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
