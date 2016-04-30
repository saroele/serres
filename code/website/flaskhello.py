from flask import Flask
app = Flask(__name__)

@app.route('/user/<username>')
def username(username):
    lookup = {'roel':'Roel De Coninck', 'lies':'Liesbeth Vansteenvoort'}
    if username in lookup:
        return 'Your full name is {}'.format(lookup[username])
    else:
        return 'Your full name is unknown'

@app.route('/')
def index():


    return 'Index Page'

@app.route('/hello')
def hello():
    return 'Hello World'

if __name__ == '__main__':
    app.run(host='0.0.0.0')
