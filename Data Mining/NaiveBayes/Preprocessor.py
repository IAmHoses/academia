import Records
from collections import Counter
import math

class Preprocessor:
	def __init__(self):
		self.class_1_list, self.class_2_list = Records.load_raw_data()
		self.max_values = self.find_all_maxes()
		self.optimal_splits = {}

	def __str__(self):
		all_records = ""

		for record in self.class_1_list:
			all_records += str(record) + '\n'
		for record in self.class_2_list:
			all_records += str(record) + '\n'

		return all_records

	def clean_data_and_export(self, file = 'preprocessed-data.arff'):
		print("Preprocessing data...")

		self.replace_values()
		print("Check -> Empty attribute values replaced with categorical modes and numerical means")
		self.discretize_all_values()
		print("Check -> Entropy-based discretization of continuous features completed")
		output = open(file, 'w')
		print("Check -> Data fully cleaned, writing to output file")

		output.write("@relation adult\n\n")
		output.write("@attribute age { <=" + str(self.optimal_splits['age']) + ", >" + str(self.optimal_splits['age']) + " }\n")
		output.write("@attribute workclass { Private, Self-emp-not-inc, Self-emp-inc, Federal-gov, Local-gov, State-gov, Without-pay, Never-worked }\n")
		output.write("@attribute fnlwgt { <=" + str(self.optimal_splits['fnlwgt']) + ", >" + str(self.optimal_splits['fnlwgt']) + " }\n")
		output.write("@attribute education { Bachelors, Some-college, 11th, HS-grad, Prof-school, Assoc-acdm, Assoc-voc, 9th, 7th-8th, 12th, Masters, 1st-4th, 10th, Doctorate, 5th-6th, Preschool }\n")
		output.write("@attribute education-num { <=" + str(self.optimal_splits['education-num']) + ", >" + str(self.optimal_splits['education-num']) + " }\n")
		output.write("@attribute marital-status { Married-civ-spouse, Divorced, Never-married, Separated, Widowed, Married-spouse-absent, Married-AF-spouse }\n")
		output.write("@attribute occupation { Tech-support, Craft-repair, Other-service, Sales, Exec-managerial, Prof-specialty, Handlers-cleaners, Machine-op-inspct, Adm-clerical, Farming-fishing, Transport-moving, Priv-house-serv, Protective-serv, Armed-Forces }\n")
		output.write("@attribute relationship { Wife, Own-child, Husband, Not-in-family, Other-relative, Unmarried }\n")
		output.write("@attribute race { White, Asian-Pac-Islander, Amer-Indian-Eskimo, Other, Black }\n")
		output.write("@attribute sex { Female, Male }\n")
		output.write("@attribute capital-gain { <=" + str(self.optimal_splits['capital-gain']) + ", >" + str(self.optimal_splits['capital-gain']) + " }\n")
		output.write("@attribute capital-loss { <=" + str(self.optimal_splits['capital-loss']) + ", >" + str(self.optimal_splits['capital-loss']) + " }\n")
		output.write("@attribute hours-per-week { <=" + str(self.optimal_splits['hours-per-week']) + ", >" + str(self.optimal_splits['hours-per-week']) + " }\n")
		output.write("@attribute native-country { United-States, Cambodia, England, Puerto-Rico, Canada, Germany, Outlying-US(Guam-USVI-etc), India, Japan, Greece, South, China, Cuba, Iran, Honduras, Philippines, Italy, Poland, Jamaica, Vietnam, Mexico, Portugal, Ireland, France, Dominican-Republic, Laos, Ecuador, Taiwan, Haiti, Columbia, Hungary, Guatemala, Nicaragua, Scotland, Thailand, Yugoslavia, El-Salvador, Trinadad&Tobago, Peru, Hong, Holand-Netherlands }\n")
		output.write("@attribute class { >50K, <=50K }\n\n")
		output.write("@data\n")
		for record in self.class_1_list:
			output.write(str(record) + '\n')
		for record in self.class_2_list:
			output.write(str(record) + '\n')

		output.close()
		print("Preprocessing complete, data written to preprocessed-data.arff\n")
		print("Optimal splits: " + str(self.optimal_splits) + '\n')

	def replace_missing_categorical(self, class_list):
		workclass_counter = Counter()
		occupation_counter = Counter()
		native_country_counter = Counter()

		for record in class_list:
			workclass_counter[record.get_workclass()] += 1
			occupation_counter[record.get_occupation()] += 1
			native_country_counter[record.get_native_country()] += 1

		workclass_mode = str(workclass_counter.most_common(1)).partition("'")[2].partition("'")[0]
		occupation_mode = str(occupation_counter.most_common(1)).partition("'")[2].partition("'")[0]
		native_country_mode = str(native_country_counter.most_common(1)).partition("'")[2].partition("'")[0]

		for record in class_list:
			if record.get_workclass() == '?':
				record.record_dict['workclass'] = workclass_mode

			if record.get_occupation() == '?':
				record.record_dict['occupation'] = occupation_mode

			if record.get_native_country() == '?':
				record.record_dict['native-country'] = native_country_mode

	def replace_values(self):
		self.replace_missing_categorical(self.class_1_list)
		self.replace_missing_categorical(self.class_2_list)

	def calculate_label_frequencies_in_bin(self, attribute, split):
		class_label_counter = Counter()

		for index1, record in enumerate(self.class_1_list):
			if int(self.class_1_list[index1].record_dict[attribute]) <= split:
				class_label_counter['class1 <='] += 1
			else:
				class_label_counter['class1 >'] += 1
		
		for index2, record in enumerate(self.class_2_list):
			if int(self.class_2_list[index2].record_dict[attribute]) <= split:
				class_label_counter['class2 <='] +=1
			else:
				class_label_counter['class2 >'] += 1

		probability_dict = {'bin attribute' : attribute, 'bin split' : split}

		probability_dict['class1 <='] = class_label_counter['class1 <='] / (index1 + 1)
		probability_dict['class1 >'] = class_label_counter['class1 >'] / (index1 + 1)
		probability_dict['class2 <='] = class_label_counter['class2 <='] / (index2 + 1)
		probability_dict['class2 >'] = class_label_counter['class2 >'] / (index2 + 1)
		probability_dict['D1/D'] = (class_label_counter['class1 <='] + class_label_counter['class1 >']) / (index1 + index2 + 2)
		probability_dict['D2/D'] = (class_label_counter['class2 <='] + class_label_counter['class2 >']) / (index1 + index2 + 2)

		return probability_dict

	def calculate_net_entropy(self, bin_frequencies):
		if bin_frequencies['class1 <='] == 0.0 and bin_frequencies['class1 >'] == 0.0:
			entropy_1 = 0.0

		elif bin_frequencies['class1 <='] == 0.0:
			entropy_1 = -bin_frequencies['class1 >'] * math.log(bin_frequencies['class1 >'],2)

		elif bin_frequencies['class1 >'] == 0.0:
			entropy_1 = -bin_frequencies['class1 <='] * math.log(bin_frequencies['class1 <='],2)

		else:
			entropy_1 = (-(bin_frequencies['class1 <='] * math.log(bin_frequencies['class1 <='],2)) - \
				(bin_frequencies['class1 >'] * math.log(bin_frequencies['class1 >'],2)))
		
		if bin_frequencies['class2 <='] == 0.0 and bin_frequencies['class2 >'] == 0.0:
			entropy_2 = 0.0

		elif bin_frequencies['class2 <='] == 0.0:
			entropy_2 = -bin_frequencies['class2 >'] * math.log(bin_frequencies['class2 >'],2)

		elif bin_frequencies['class2 >'] == 0.0:
			entropy_2 = -bin_frequencies['class2 <='] * math.log(bin_frequencies['class2 <='],2)

		else:
			entropy_2 = (-(bin_frequencies['class2 <='] * math.log(bin_frequencies['class2 <='],2)) - \
				(bin_frequencies['class2 >'] * math.log(bin_frequencies['class2 >'],2)))
		
		net_entropy = (bin_frequencies['D1/D'] * entropy_1) + (bin_frequencies['D2/D'] * entropy_2)
		return net_entropy

	def calculate_entropy_gain(self, bin_frequencies, initial_entropy = 1):
		return initial_entropy - self.calculate_net_entropy(bin_frequencies)

	def find_max_value(self, attribute):
		max_value = 0

		for record in self.class_1_list:
			if int(record.record_dict[attribute]) > max_value:
				max_value = int(record.record_dict[attribute])
		for record in self.class_2_list:
			if int(record.record_dict[attribute]) > max_value:
				max_value = int(record.record_dict[attribute])

		return max_value

	def find_all_maxes(self):
		dict_of_maxes = {'age' : 0, 'fnlwgt' : 0, 'education-num' : 0, 'capital-gain' : 0, \
			'capital-loss' : 0, 'hours-per-week' : 0}
		
		for key in dict_of_maxes:
			dict_of_maxes[key] = self.find_max_value(key)

		return dict_of_maxes

	def discretize_by_entropy(self, attribute, max_gain, best_split, split_factor = 0.60):
		if split_factor == 0.35 or max_gain >= 0.40:
			return best_split

		new_split = self.max_values[attribute] - (self.max_values[attribute] * split_factor)
		bin_frequencies = self.calculate_label_frequencies_in_bin(attribute, new_split)
		current_info_gain = self.calculate_entropy_gain(bin_frequencies)

		if current_info_gain > max_gain:
			max_gain = current_info_gain
			best_split = new_split

		return self.discretize_by_entropy(attribute, max_gain, best_split, split_factor - 0.05)

	def discretize_all_values(self):
		dict_of_splits = {'age' : 0, 'fnlwgt' : 0, 'education-num' : 0, 'capital-gain' : 0, \
			'capital-loss' : 0, 'hours-per-week' : 0}
		
		for key in dict_of_splits:
			dict_of_splits[key] = round(self.discretize_by_entropy(key, 0, 0), 2)

		for record in self.class_1_list:
			for key in dict_of_splits:
				if float(record.record_dict[key]) <= dict_of_splits[key]:
					record.record_dict[key] = ("<=" + str(dict_of_splits[key]))
				else:
					record.record_dict[key] = (">" + str(dict_of_splits[key]))

		for record in self.class_2_list:
			for key in dict_of_splits:
				if float(record.record_dict[key]) <= dict_of_splits[key]:
					record.record_dict[key] = ("<=" + str(dict_of_splits[key]))
				else:
					record.record_dict[key] = (">" + str(dict_of_splits[key]))

		self.optimal_splits = dict_of_splits