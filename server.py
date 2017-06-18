
#!/usr/local/bin/python3

from flask import Flask
from flask_restful import Api, Resource
from flask import request
from flask.json import jsonify
from flask_cache import Cache
from flask import Flask, session
from flask_session import Session
import course_lib

app = Flask(__name__)
api = Api(app)
sess = Session()

class CourseAPI(Resource):
    def get(self):
        return course_lib.get_selected_courses(session['cookie_value'], session['student_id'])
    def post(self):
        _id = request.form.get('CrsId')
        _type = request.form.get('CrsType')
        _pri = request.form.get('priority')
        course_lib.course_select(session['cookie_value'], _id, _type, _pri)
        all_courses = course_lib.get_selected_courses(session['cookie_value'], session['student_id'])
        for course in all_courses["data"]:
            if course[0] == _id: 
                return{"success": True, "selected_course": course[0]}
        return {"success": False}
    def put(self):
        _id = request.form.get('CrsId')
        _type = request.form.get('CrsType')
        course_lib.course_unselect(session['cookie_value'], _id, _type)
        all_courses = course_lib.get_selected_courses(session['cookie_value'], session['student_id'])
        for course in all_courses["data"]:
            if course[0] == _id: 
                return {"success": False}
        return {"success": True, "unselected_course": _id}

@app.route('/')
def index():
    return "Happy hacking!"

@app.route('/api/fake_login', methods = ['post'])
def fake_login():
    student_ID = request.form.get('id')
    password = request.form.get('password')
    if student_ID == None or password == None:
        return jsonify({"Error": "id and password should not be blank"}), 403
    cookie = course_lib.refresh_cookie(student_ID)
    qCode = course_lib.crack_captcha(cookie)
    success = course_lib.login(cookie, student_ID, password, qCode)
    if success:
        session['cookie_value'] = cookie
        session['student_id'] = student_ID
        session['password'] = password
    return jsonify({"success": success})


if __name__ == '__main__':
    app.secret_key = 'NCTU948794crazy'
    app.config['SESSION_TYPE'] = 'filesystem'
    sess.init_app(app)

    api.add_resource(CourseAPI, '/api/course/', endpoint = 'course')
    app.run(host="0.0.0.0", port=9487, debug=True)
