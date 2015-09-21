#!/usr/bin/env python


def launch(app_cmd, plugin, newcoordinator, port, modifyenv, daemon, interval, envvarmap):
	import utils
	import constants
	import subprocess as s

	if envvarmap == None:
		envvarmap = {'STATFILE' : '', 'STATGEN' : ''}

	cmd = 'STATFILE=' + envvarmap['STATFILE'] + ' ' + 'STATGEN=' + envvarmap['STATGEN'] + ' '

	cmd += constants.DMTCP_LAUNCH 

	if (interval != None) and ('-i' in interval):
		cmd += (' ' + interval)

	if (plugin != None) and ('--with-plugin' in plugin):
		cmd += (' ' + plugin)
		if 'myplug' in cmd:
			cmd = cmd.replace('myplug', constants.MYPLUG) 

	if (newcoordinator != None) and ('--new-coordinator' == newcoordinator):
		cmd += (' ' + newcoordinator)

	if (port != None) and ('--port' in port):
		cmd += (' ' + port)

	if (modifyenv != None) and ('--modify-env' == modifyenv):
		cmd += (' ' + modifyenv)

	cmd += (' --no-gzip')

	if ('FMM' in app_cmd) or ('WATER-NSQUARED' in app_cmd):
		cmd = app_cmd.split(';')[0] + ' ; ' + cmd + app_cmd.split(';')[1]
	else:
		cmd += (' ' + app_cmd)

	cmd += (' >> ' + constants.LOGDIR + '/' + constants.LOGGER + ' 2>&1 ')

	utils.loginfo('Final launch command: ' + cmd)

	obj = None

	if (daemon != None) and ('--daemon' == daemon):
		obj = s.Popen(cmd, shell=True)
	else:
		obj = s.Popen(cmd, shell=True).wait()
		
	return obj
	
def chkpt(port, blocked):
	import utils
	import constants

	cmd = constants.DMTCP_COMMAND

	if (blocked != None) and (blocked == '-b'):
		cmd += ' -bc'
	else:
		cmd += ' -c'

	if (port != None) and ('--port' in port):
		cmd += (' ' + port)
	
	cmd += (' >> ' + constants.LOGDIR + '/' + constants.LOGGER + ' 2>&1 ')

	utils.execcmd(cmd)

def kill(port):
	import utils
	import constants

	cmd = constants.DMTCP_COMMAND + ' -k'

	if (port != None) and ('--port' in port):
		cmd += (' ' + port)

	cmd += (' >> ' + constants.LOGDIR + '/' + constants.LOGGER + ' 2>&1 ')

	utils.execcmd(cmd)

def restart(chkpt_img, newcoordinator, port, daemon, interval, envvarmap):
	import utils
	import constants
	import subprocess as s

	perf_stat = ''

	cmd = constants.DMTCP_RESTART

	if (newcoordinator != None) and ('--new-coordinator' == newcoordinator):
                cmd += (' ' + newcoordinator)

	if (port != None) and ('--port' in port):
                cmd += (' ' + port)
	
	if (interval != None) and ('-i' in interval):
		cmd += (' ' + interval)

	cmd += (' ' + chkpt_img)

	if (daemon == None) and (envvarmap['STATGEN'] == ''):
		perf_stat = ' perf stat '
		cmd += (' >> ' + envvarmap['STATFILE'] + ' 2>&1 ')
		envvarmap['STATFILE'] = ''
	else:
		cmd += (' >> ' + constants.LOGDIR + '/' + constants.LOGGER + ' 2>&1 ')

	cmd = 'STATFILE=' + envvarmap['STATFILE'] + ' ' + 'STATGEN=' + envvarmap['STATGEN'] + ' ' + perf_stat + cmd

	utils.loginfo(cmd)	


	obj = None

	if (daemon != None) and ('--daemon' == daemon):
		obj = s.Popen(cmd, shell=True)
	else:
		obj = s.Popen(cmd, shell=True).wait()
		
	return obj

