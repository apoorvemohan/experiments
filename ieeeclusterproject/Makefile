default: plugin

vim:
	vim dmtcp/src/plugin/myplug/myplug.cpp

plugin:
	cd dmtcp/src/plugin/myplug/ && make

plugin-clean:
	cd dmtcp/src/plugin/myplug/ && make clean

run:
	bin/init_app_and_chkpt -p

clean:
	rm -rf /pool001/jiajun/apoorve/chkpts*
	rm -rf data/parallel_dmtcp/stats/*
	rm -f log/init_app_and_chkpt_a.log 
	rm -f log/parallel_dmtcp_a.log 
	rm -rf data/parallel_dmtcp/stats*
