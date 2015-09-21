#!/usr/bin/env python

def getchkptimg(folder):
	import os
	for item in os.listdir(folder):
		if '.dmtcp' in item:
			return item

def listonlydir(path):
	import os
	return [d for d in os.listdir(path) if os.path.isdir(os.path.join(path, d))]

def execcmd(command):
	import os
	os.system(command)

def execcmdbg(command):
	import os
	os.system(command + ' &')

def mkdir(dirname):
	import os	
	os.mkdir(dirname)

def chdir(dirname):
	import os
	os.chdir(dirname)

def isfile(filename):
	import os
	return os.path.isfile(filename)

def isdir(dirname):
	import os
	os.path.isdir(dirname)

def gettimestamp():
	import datetime
	return str(datetime.datetime.now()).split('.')[0]

def getcwd():
	import os
	return os.getcwd()

def getopenport():
        import socket
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.bind(('',0))
        s.listen(1)
        port = s.getsockname()[1]
        s.close()
        return port

def getidcmdpairbyxml(xmlfile):
	import xml.etree.ElementTree as ET
	import sys
	import os

	path = os.path.dirname(os.path.realpath(__file__))  + '/../..'
	sys.path.insert(0, path)

	import data.app_commands as ac

	tree = ET.parse(xmlfile)
	root = tree.getroot()

	key = root.get('ID')
	app = root.get('NAME')
	cmd = ac.APP_CMD_MAP[app]

	for arg in root.getchildren():
		cmd = cmd.replace(arg.tag, arg.text)

	return {key:cmd}
	

def getappidlist():
	import xml.etree.ElementTree as ET

	import constants

	tree = ET.parse(constants.DATAFILE)
	root = tree.getroot()
	return [attr.attrib.get('ID') for attr in root.iter('APPLICATIONDATA')]

def getappexeccommandbyid(app_id):
	import xml.etree.ElementTree as ET

        import constants

        tree = ET.parse(constants.DATAFILE)
        root = tree.getroot()
	app_name = None
	app_data = None

	for node in root.findall('APPLICATIONDATA'):
		if node.get('ID') == app_id:
			app_name = node.get('NAME')
			app_data = node
			break
	
	app_cmd = None

	for node in root.findall('APPLICATION'):
		if node.get('NAME') == app_name:
			app_cmd = node.text
			break

	app_cmd = app_cmd.replace(app_name+'_PATH', constants.APP_PATH_MAP[app_name])

	for node in app_data.getchildren():
		app_cmd = app_cmd.replace(node.tag, node.text)

	return app_cmd

def zipdir(path, zip):
	import os
	import zipfile

	for root, dirs, files in os.walk(path):
		for file in files:
			zip.write(os.path.join(root, file))

def loginfo(info):
	import constants
	fp = open(constants.LOGDIR + '/' + constants.LOGGER, 'a+')
	fp.write('[' + gettimestamp()+' INFO] ' + str(info) + '\n')
	fp.close()

def logerr(info):
	import constants
	fp = open(constants.LOGDIR + '/' + constants.LOGGER, 'a+')
	fp.write('[' + gettimestamp()+' ERROR] ' + str(info) + '\n')
	fp.close()

def logwarn(info):
	import constants
	fp = open(constants.LOGDIR + '/' + constants.LOGGER, 'a+')
	fp.write('[' + gettimestamp()+' WARNING] ' + str(info) + '\n')
	fp.close()
