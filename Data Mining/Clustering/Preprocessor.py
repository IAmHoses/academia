import Records
from collections import Counter
import math
import copy

class Preprocessor:
	def __init__(self):
		raw_data = Records.load_raw_data()
		self.all_records = raw_data[0]
		self.class_1_records = raw_data[1]
		self.class_2_records = raw_data[2]

		self.means = {'age': 0, 'fnlwgt': 0, 'education-num': 0, 'capital-gain': 0, \
			'capital-loss': 0, 'hours-per-week': 0}

		self.std_devs = {'age': 0, 'fnlwgt': 0, 'education-num': 0, 'capital-gain': 0, \
			'capital-loss': 0, 'hours-per-week': 0}

		self.num_loaded = len(self.all_records)

	def __str__(self):
		output = ""
		for record in self.all_records:
			output += '%s\n' % str(record)
		return output

	def preprocess_data(self, file = 'preprocessed-data.arff'):
		print("Preprocessing data...")

		self.replace_values()
		print("Check -> Empty attribute values replaced with categorical modes")

		self.normalize_by_z_score()
		print("Check -> Numerical attributes normalized by Z-score and outliers removed")

		output = open(file, 'w')
		print("Check -> Data fully cleaned, writing to output file\n")

		output.write("@relation adult\n\n")
		output.write("@attribute age numeric\n")
		output.write("@attribute workclass { Private, Self-emp-not-inc, Self-emp-inc, Federal-gov, Local-gov, State-gov, Without-pay, Never-worked }\n")
		output.write("@attribute fnlwgt numeric\n")
		output.write("@attribute education { Bachelors, Some-college, 11th, HS-grad, Prof-school, Assoc-acdm, Assoc-voc, 9th, 7th-8th, 12th, Masters, 1st-4th, 10th, Doctorate, 5th-6th, Preschool }\n")
		output.write("@attribute education-num numeric\n")
		output.write("@attribute marital-status { Married-civ-spouse, Divorced, Never-married, Separated, Widowed, Married-spouse-absent, Married-AF-spouse }\n")
		output.write("@attribute occupation { Tech-support, Craft-repair, Other-service, Sales, Exec-managerial, Prof-specialty, Handlers-cleaners, Machine-op-inspct, Adm-clerical, Farming-fishing, Transport-moving, Priv-house-serv, Protective-serv, Armed-Forces }\n")
		output.write("@attribute relationship { Wife, Own-child, Husband, Not-in-family, Other-relative, Unmarried }\n")
		output.write("@attribute race { White, Asian-Pac-Islander, Amer-Indian-Eskimo, Other, Black }\n")
		output.write("@attribute sex { Female, Male }\n")
		output.write("@attribute capital-gain numeric\n")
		output.write("@attribute capital-loss numeric\n")
		output.write("@attribute hours-per-week numeric\n")
		output.write("@attribute native-country { United-States, Cambodia, England, Puerto-Rico, Canada, Germany, Outlying-US(Guam-USVI-etc), India, Japan, Greece, South, China, Cuba, Iran, Honduras, Philippines, Italy, Poland, Jamaica, Vietnam, Mexico, Portugal, Ireland, France, Dominican-Republic, Laos, Ecuador, Taiwan, Haiti, Columbia, Hungary, Guatemala, Nicaragua, Scotland, Thailand, Yugoslavia, El-Salvador, Trinadad&Tobago, Peru, Hong, Holand-Netherlands }\n")
		output.write("@attribute class { >50K, <=50K }\n\n")
		output.write("@data\n")

		for record in self.all_records:
			output.write('%s' % str(record))

		output.close()
		print("Preprocessing complete, data written to preprocessed-data.arff")
		print("%s records remaining after preprocessing, %s outliers removed" % \
			(str(len(self.all_records)), str(self.num_loaded - len(self.all_records))))

	"""
		replace_missing_categorical(self, class_list)
			Scans a list of records of a single class, computing modes of categorical
			attributes with missing values while doing so. Records missing a value for
			any one of these attributes are then filled in with its mode. This function 
			serves as a helper method for replace_values(self), which simply calls 
			replace_missing_categorical twice -- once with each class's list of records
	"""

	def replace_missing_categorical(self, class_list):
		workclass_counter = Counter()
		occupation_counter = Counter()
		native_country_counter = Counter()

		for record in class_list:
			workclass_counter[record.workclass()] += 1
			occupation_counter[record.occupation()] += 1
			native_country_counter[record.native_country()] += 1

		workclass_mode = workclass_counter.most_common(1)[0][0]
		occupation_mode = occupation_counter.most_common(1)[0][0]
		native_country_mode = native_country_counter.most_common(1)[0][0]

		for record in class_list:
			if record.workclass() == '?':
				record.cat_attributes['workclass'] = workclass_mode

			if record.occupation() == '?':
				record.cat_attributes['occupation'] = occupation_mode

			if record.native_country() == '?':
				record.cat_attributes['native-country'] = native_country_mode

	def replace_values(self):
		self.replace_missing_categorical(self.class_1_records)
		self.replace_missing_categorical(self.class_2_records)

	def compute_means(self):
		#age_total, fnlwgt_total, education_num_total, capital_gain_total, capital_loss_total, hours_per_week_total = 0

		attribute_totals = {'age': 0, 'fnlwgt': 0, 'education-num': 0, 'capital-gain': 0, \
			'capital-loss': 0, 'hours-per-week': 0}

		for i, record in enumerate(self.all_records):
			for attribute in self.means:
				attribute_totals[attribute] += float(self.all_records[i].num_attributes[attribute])

		for attribute in self.means:
			self.means[attribute] = attribute_totals[attribute] / len(self.all_records)

	def compute_std_devs(self):
		squared_errors = {'age': 0, 'fnlwgt': 0, 'education-num': 0, 'capital-gain': 0, \
			'capital-loss': 0, 'hours-per-week': 0}

		for i, record in enumerate(self.all_records):
			for attribute in self.std_devs:
				squared_errors[attribute] += pow(float(self.all_records[i].num_attributes[attribute]) - \
					self.means[attribute], 2)

		for attribute in self.std_devs:
			self.std_devs[attribute] = math.sqrt(squared_errors[attribute] / len(self.all_records))

	def normalize_by_z_score(self):
		self.compute_means()
		self.compute_std_devs()

		records_copy = copy.copy(self.all_records)

		for record in records_copy:
			for attribute in record.num_attributes:
				z_score = record.num_attributes[attribute] = \
					(float(record.num_attributes[attribute]) - self.means[attribute]) / self.std_devs[attribute]

				# check to see if the record contains an outlier, and remove if so
				if abs(z_score) > 3:
					self.all_records.remove(record)
					break

				else:
					record.num_attributes[attribute] = str(z_score)
 
	def remove_outliers(self):
		for record in self.all_records:
			for attribute in record.num_attributes:
				if abs(float(record.num_attributes[attribute])) > 3:
					self.all_records.remove(record)
					break

def main():
	p = Preprocessor()
	p.preprocess_data()

if __name__ == '__main__':
	main()
