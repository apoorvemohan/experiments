default:

vim:
	vim dmtcp/src/plugin/myplug/myplug.c

plugin:
	cd dmtcp/src/plugin/myplug/ && make

run:
	bin/init_app_and_chkpt -p

clean:
	rm -rf /pool001/jiajun/apoorve/chkpts*
	rm -rf data/parallel_dmtcp/stats/*
	rm -f log/init_app_and_chkpt_a.log 
	rm -f log/parallel_dmtcp_a.log 
	rm -rf data/parallel_dmtcp/stats*
