
class Record:
	"""
		a simple container for record attributes stored in each line of adult-big.arff
	"""

	def __init__(self, age, workclass, fnlwgt, education, education_num, marital_status,
		occupation, relationship, race, sex, capital_gain, capital_loss, hours_per_week,
		native_country, class_label):

		self.num_attributes = {}
		self.cat_attributes = {}

		self.num_attributes['age'] = age
		self.num_attributes['fnlwgt'] = fnlwgt
		self.num_attributes['education-num'] = education_num
		self.num_attributes['capital-gain'] = capital_gain
		self.num_attributes['capital-loss'] = capital_loss
		self.num_attributes['hours-per-week'] = hours_per_week

		self.cat_attributes['workclass'] = workclass
		self.cat_attributes['education'] = education
		self.cat_attributes['marital-status'] = marital_status
		self.cat_attributes['occupation'] = occupation
		self.cat_attributes['relationship'] = relationship
		self.cat_attributes['race']= race
		self.cat_attributes['sex'] = sex
		self.cat_attributes['native-country'] = native_country

		self.class_label = class_label

	def __str__(self):
		return "%s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s\n" % (self.age(), self.workclass(), \
			self.fnlwgt(), self.education(), self.edu_num(), self.marital_status(), self.occupation(), \
			self.relationship(), self.race(), self.sex(), self.cap_gain(), self.cap_loss(), self.hours_per_week(), \
			self.native_country(), self.class_label)

	def age(self):
		return self.num_attributes['age']

	def workclass(self):
		return self.cat_attributes['workclass']

	def fnlwgt(self):
		return self.num_attributes['fnlwgt']

	def education(self):
		return self.cat_attributes['education']

	def edu_num(self):
		return self.num_attributes['education-num']

	def marital_status(self):
		return self.cat_attributes['marital-status']

	def occupation(self):
		return self.cat_attributes['occupation']

	def relationship(self):
		return self.cat_attributes['relationship']

	def race(self):
		return self.cat_attributes['race']

	def sex(self):
		return self.cat_attributes['sex']

	def cap_gain(self):
		return self.num_attributes['capital-gain']

	def cap_loss(self):
		return self.num_attributes['capital-loss']

	def hours_per_week(self):
		return self.num_attributes['hours-per-week']

	def native_country(self):
		return self.cat_attributes['native-country']

"""
	load_raw_data(file)
		Reads from adult-big.arff and imports all of its raw data into a usable form for preprocessing.
		The method stores attribute values of each record in a unique Record object, then places the 
		object in a list.
"""

def load_raw_data(file = 'adult-big.arff'):
	all_records, class_1_records, class_2_records = [], [], []	# container for Record objects

	with open(file, 'r') as raw_data:
		print("Loading raw data...")
		
		for line in raw_data.readlines():
			if line == '\n' or line[0] == '@':
				continue
			
			else:
				parsed = line.split(', ')

				new_record = Record(parsed[0], parsed[1], parsed[2], parsed[3], parsed[4], parsed[5], parsed[6], \
					parsed[7], parsed[8], parsed[9], parsed[10], parsed[11], parsed[12], parsed[13], parsed[14][:-1])

				all_records.append(new_record)

				if new_record.class_label == '<=50K':
					class_2_records.append(new_record)
				else:
					class_1_records.append(new_record)

	print("%s records loaded into Preprocessor!\n" % str(len(all_records)))
	return (all_records, class_1_records, class_2_records)


def load_preprocessed_data(file = 'preprocessed-data.arff'):
	preprocessed_records, class_1_records, class_2_records = [], [], []		# container for Record objects

	with open(file, 'r') as preprocessed_data:
		print("Loading preprocessed data...")

		for line in preprocessed_data.readlines():
			if line == '\n' or line[0] == '@':
				continue
			
			else:
				parsed = line.split(', ')

				new_record = Record(parsed[0], parsed[1], parsed[2], parsed[3], parsed[4], parsed[5], parsed[6], \
					parsed[7], parsed[8], parsed[9], parsed[10], parsed[11], parsed[12], parsed[13], parsed[14][:-1])

				preprocessed_records.append(new_record)

				if new_record.class_label == '<=50K':
					class_2_records.append(new_record)
				else:
					class_1_records.append(new_record)				

	print("%s records loaded into Buckshot clusterer!\n" % str(len(preprocessed_records)))
	return (preprocessed_records, class_1_records, class_2_records)


def main():
	load_raw_data()

if __name__ == '__main__':
	main()