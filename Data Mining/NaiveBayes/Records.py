"""
	Record
		a simple container for data in each line under @data in adult-big.arff
"""

class Record:
	def __init__(self, age, workclass, fnlwgt, education, education_num, marital_status,
		occupation, relationship, race, sex, capital_gain, capital_loss, hours_per_week,
		native_country, class_label):

		self.record_dict = {}

		self.record_dict['age'] = age
		self.record_dict['workclass'] = workclass
		self.record_dict['fnlwgt'] = fnlwgt
		self.record_dict['education'] = education
		self.record_dict['education-num'] = education_num
		self.record_dict['marital-status'] = marital_status
		self.record_dict['occupation'] = occupation
		self.record_dict['relationship'] = relationship
		self.record_dict['race']= race
		self.record_dict['sex'] = sex
		self.record_dict['capital-gain'] = capital_gain
		self.record_dict['capital-loss'] = capital_loss
		self.record_dict['hours-per-week'] = hours_per_week
		self.record_dict['native-country'] = native_country
		self.record_dict['class-label'] = class_label

	def __str__(self):
		return self.record_dict['age'] + ', ' + self.record_dict['workclass'] + ', ' + \
		self.record_dict['fnlwgt'] + ', ' + self.record_dict['education'] + ', ' + \
		self.record_dict['education-num'] + ', ' + self.record_dict['marital-status'] + ', ' + \
		self.record_dict['occupation'] + ', ' + self.record_dict['relationship'] + ', ' + \
		self.record_dict['race'] + ', ' + self.record_dict['sex'] + ', ' + \
		self.record_dict['capital-gain'] + ', ' + self.record_dict['capital-loss'] + ', ' + \
		self.record_dict['hours-per-week'] + ', ' + self.record_dict['native-country'] + ', ' + \
		self.record_dict['class-label']

	def get_age(self):
		return self.record_dict['age']

	def get_workclass(self):
		return self.record_dict['workclass']

	def get_fnlwgt(self):
		return self.record_dict['fnlwgt']

	def get_education(self):
		return self.record_dict['education']

	def get_education_num(self):
		return self.record_dict['education-num']

	def get_marital_status(self):
		return self.record_dict['marital-status']

	def get_occupation(self):
		return self.record_dict['occupation']

	def get_relationship(self):
		return self.record_dict['relationship']

	def get_sex(self):
		return self.record_dict['sex']

	def get_capital_gain(self):
		return self.record_dict['capital-gain']

	def get_capital_loss(self):
		return self.record_dict['capital-loss']

	def get_native_country(self):
		return self.record_dict['native-country']

	def get_class_label(self):
		return self.record_dict['class-label']

"""
	load(file)
		Reads from adult-big.arff and imports its data into a usable form for preprocessing.
		The method will store data for each record in a Record object, then place the record 
		in one of two arrays: class_1_list (>50k) and class_2_list (<=50k)
"""

def load_raw_data(file = 'adult-big.arff'):
	class_1_list, class_2_list = [], []		# container for Record objects. class 1 <=50k, class 2 >50k

	with open(file, 'r') as raw_data:
		print("Loading raw data...")
		
		for line in raw_data.readlines():
			if line == "" or line[0] == '@':
				continue

			else:
				parsed = line.split()
				if parsed == []:	continue
			
				else:
					new_record = Record((parsed[0])[:-1], (parsed[1])[:-1], (parsed[2])[:-1], (parsed[3])[:-1], 
						(parsed[4])[:-1], (parsed[5])[:-1], (parsed[6])[:-1], (parsed[7])[:-1], (parsed[8])[:-1], 
						(parsed[9])[:-1], (parsed[10])[:-1], (parsed[11])[:-1], (parsed[12])[:-1], (parsed[13])[:-1], 
						(parsed[14]))
					if new_record.get_class_label() == '>50K':	class_1_list.append(new_record)
					if new_record.get_class_label() == '<=50K':	class_2_list.append(new_record)
				
	print(str(len(class_1_list) + len(class_2_list)) + " records loaded into Preprocessor!\n")
	return class_1_list, class_2_list

def load_preprocessed_data(file = 'preprocessed-data.arff'):
	imported_records = []

	with open(file, 'r') as preprocessed_data:
		print("\tLoading preprocessed data...")

		for line in preprocessed_data.readlines():
			if line == "" or line[0] == '@':
				continue

			else:
				parsed = line.split()
				if parsed == []:	
					continue

				else:
					new_record = Record((parsed[0])[:-1], (parsed[1])[:-1], (parsed[2])[:-1], (parsed[3])[:-1], 
						(parsed[4])[:-1], (parsed[5])[:-1], (parsed[6])[:-1], (parsed[7])[:-1], (parsed[8])[:-1], 
						(parsed[9])[:-1], (parsed[10])[:-1], (parsed[11])[:-1], (parsed[12])[:-1], (parsed[13])[:-1], 
						(parsed[14]))
					imported_records.append(new_record)

	print('\t' + str(len(imported_records)) + " records loaded into Naive Bayes classifier!")
	return imported_records