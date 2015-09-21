import time
import signal
#import subprocess32 as s
import subprocess as s


def a(b,c=None):
	print b
	print c

a(1)
a(2,3)

'''#signal.signal(signal.SIGCHLD, signal.SIG_IGN)
a = s.Popen('sleep 50', shell=True)
x=1
while a.poll() != 0:
        time.sleep(15)
        print x
        x += 1

print 'finish'


func = [test, test1]

import random
import sys

b = random.choice(func)

b(1)'''
