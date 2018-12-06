plot: data.csv 
	python plot.py

data.csv:
	python gen_tests.py < spec | python collect_data.py

.PHONY: clean
clean:
	rm data.csv
