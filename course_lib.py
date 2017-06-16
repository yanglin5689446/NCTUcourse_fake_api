#!/usr/local/bin/python3

import constant as constant
import config as config
from bs4 import BeautifulSoup as BS
import urllib.request as ur
import urllib.parse as up
import os
import json
import subprocess


def refresh_cookie(ID):
    cookie = ""
    headers = ur.urlopen(constant.HOST).headers.get_all('Set-Cookie')
    for header in headers:
        cookie +=header.split(' ')[0]
        cookie += ' '
    cookie += 'UserID='+ID+';'
    return cookie
    
    
def crack_captcha(cookie):
    captcha_request = ur.Request(constant.HOST + constant.CAPTCHA)
    captcha_request.add_header('Cookie', cookie)
    captcha_img = ur.urlopen(captcha_request).read()
    with open('captcha.bmp', 'w+b') as f:
        f.write(captcha_img)
    result = subprocess.run(['./cracker', 'captcha.bmp'], stdout=subprocess.PIPE)
    os.system("rm captcha.bmp")
    safecode = result.stdout
    return safecode 
    
def check_login_success(page_source):
    target_title = [u'交通大學課程選課系統 Online Course Registration System', u'國立交通大學智慧財產權宣導']

    try:
        page = BS(page_source, 'html.parser')
    except:
        return False
    else:
        return page.find('title').string in target_title
def login(cookie, ID, passwd, qCode):
    login_headers = [('Cookie', cookie), ('Content-Type', 'application/x-www-form-urlencoded')]
    login_data = up.urlencode([("ID", ID), ("passwd", passwd), ("qCode", qCode)]).encode('utf-8')
    login_request = ur.Request(constant.HOST + constant.LOGIN_ENTRY, data=login_data, method='POST')
    for header in login_headers:
        login_request.add_header(header[0], header[1])
    login_response = ur.urlopen(login_request)
    return check_login_success(login_response.read())


def course_operation(cookie, magic_number):
    course_headers = [
        ('Cookie', cookie), 
        ('Content-Type', 'application/x-www-form-urlencoded'),
        ('User-Agent', ' ')
    ]
    course_data = up.urlencode([('Choice', magic_number), ('CrsType', ' '), ('CategoryGroup', ' ')]).encode('utf-8')
    course_request = ur.Request(constant.HOST + constant.COURSE_SELECT_ENTRY, data=course_data, method='POST')
    for header in course_headers:
        course_request.add_header(header[0], header[1])
    course_response = ur.urlopen(course_request)

def course_select(cookie, number, _type, order=1):
    course_operation(cookie, str(_type) + str(order) + "0" + str(number))
def course_unselect(cookie, number, _type):
    course_operation(cookie, str(_type) + "01" + str(number))
def get_selected_courses(cookie, ID):
    course_headers = [
        ('Cookie', cookie), 
        ('Content-Type', 'application/x-www-form-urlencoded'),
        ('User-Agent', ' '),
        ('Referer', 'https://course.nctu.edu.tw/inMenu.asp')
    ]
    course_data = up.urlencode([('qKey', ID)]).encode('utf-8')
    course_request = ur.Request(constant.HOST + constant.COURSE_LIST_PAGE, data=course_data, method='POST')
    for header in course_headers:
        course_request.add_header(header[0], header[1])
    course_response = ur.urlopen(course_request)
    course_page = BS(course_response.read().decode('big5'), 'html5lib')
    main_table = course_page.center.find_all('table')[1].find_all('tr')[2:]
    data = []
    j = []
    for tr in main_table:
        for td in tr.find_all('td'):
            j.append(td.font.text.strip())
        if len(j) > 1:
            data.append(j)
        j = []
    response_json = { "length": len(data),  "data": data}
    return response_json
