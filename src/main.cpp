#include <bits/stdc++.h>
#include "decision_tree.h"

std::vector<Example> getExamples(const std::string& fileloc, const std::vector<std::string>& attr_names) {
	std::vector<Example> ret;
	std::vector<std::vector<std::string>> data = Reader::readData(fileloc);
	for (ll i = 0; i < data.size(); i++) {
		std::string target_value = data[i][data[i].size() - 2];
		data[i].pop_back();
		data[i].pop_back();
		ret.push_back(Example(attr_names, data[i], target_value));
	}
	return ret;
}

std::vector<Example> getTestData(const std::string& fileloc,
	const std::vector<std::string>& attr_names, bool fill_unknown) {

	std::vector<Example> els = getExamples(fileloc, attr_names);

 	if (fill_unknown) {
	  for (auto const& attr_name: attr_names) {
		  //       target_val ,      attr_val, cnt
			std::map<std::string, std::map<std::string, ll>> bins;
			for (ll i = 0; i < els.size(); i++) {
				if (els[i][attr_name] != missing_attr) {
					bins[els[i].getTargetClass()][els[i][attr_name]]++;
				}
			}

			// filling of missing vals
			for (ll i = 0; i < els.size(); i++) {
				if (els[i][attr_name] == missing_attr) {
					ll max = -1;
					std::string attr_val;
					for (auto const &x: bins[els[i].getTargetClass()]) {
						if (x.second > max) {
							max = x.second;
							attr_val = x.first;
						}
					}
					els[i].setAttrVal(attr_name, attr_val);
				}
			}
		}
 	}

	return els;
}

std::vector<std::string> getAttrNames(std::vector<std::vector<std::string>>& dat) {
	std::vector<std::string> attr_names;
	for (ll i = 0; i < dat.size(); i++) {
		attr_names.push_back(dat[i][0]);
	}
	return attr_names;
}

void fillAttrInfo(std::vector<std::vector<std::string>>& dat, DecisionTree& dt){
	for (ll i = 0; i < dat.size(); i++) {
		std::vector<std::string> temp;
		for (ll j = 1; j < dat[i].size(); j++) {
			if (dat[i][j] == "continuous") {
				break;
			} else {
				temp.push_back(dat[i][j]);
			}
		}
		dt.addAttrInfo(dat[i][0], temp);
	}
}

int main(){

	std::vector<std::string> target_values;
	std::vector<std::string> attr_names;
	target_values.push_back(">50K");
	target_values.push_back("<=50K");

	std::vector<std::vector<std::string>> dat = Reader::readData("../data/adult_attr");
	attr_names = getAttrNames(dat);

	std::vector<Example> prune_data = getTestData("../data/adult_data_prune", attr_names, true);
	std::vector<Example> test_data = getTestData("../data/adult_test", attr_names, true);
	std::vector<Example> examples=getExamples("../data/adult_data_train", attr_names);

	DecisionTree dt;

	dt.addTargetValues(target_values);

	fillAttrInfo(dat, dt);

	auto t1=std::time(NULL);
	dt.build(examples);
	auto t2=std::time(NULL);

	std::cout<<"Task 1. ID3"<<"\n";
	dt.printStats(test_data);
	std::cout<<"Took "<<t2-t1<<" seconds"<<"\n\n";

	t1=std::time(NULL);
	dt.prune(prune_data);
	t2=std::time(NULL);

	std::cout<<"Task 2. C4.5/Pruning"<<"\n";
	dt.printStats(test_data);
	std::cout<<"Took "<<t2-t1<<" seconds"<<"\n\n";

	int i=5000;
	RandomForest rf(i);
	rf.addTargetValues(target_values);
	fillAttrInfo(dat, rf);

	t1=std::time(NULL);
	rf.build(examples);
	t2=std::time(NULL);
	std::cout<<"Task 3. Random Forests with "<<i<<" trees"<<"\n";
	rf.printStats(test_data);
	std::cout<<"Took "<<t2-t1<<" seconds"<<"\n";

	return 0;
}