from flask import Flask, render_template
from server.weather import GetTemperature, GetEmails, GetNews, GetCondition, GetWeatherIcon, GetTime, GetDate, GetDayname

app = Flask(__name__, static_url_path='/static')

@app.route('/')

def hello():
    print(GetEmails()[4])
    if int(GetEmails()[4]) == 0:
        bottom = 'Die aktuellsten News:\n %s' % str(GetNews())
        index = "index"
    elif int(GetEmails()[4]) == 1:
        bottom = '%s neue Email:\n%s: %s' % (str(GetEmails()[4]), str(GetEmails()[0]), str(GetEmails()[1]))
        index = "indexmail"
    else:
        bottom = '%s neue Emails:\n%s: %s\n%s: %s' % (str(GetEmails()[4]), str(GetEmails()[0]), str(GetEmails()[1]), str(GetEmails()[2]), str(GetEmails()[3]))
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
