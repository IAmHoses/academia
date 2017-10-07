"""
	Must haves:
		(1) Training Data list
		(2) Testing Data list
		(3) Probability Dictionaries:
				- Base rates (probability of class 1 or class 2) --> 1 dictionary
				- Evidence (probabilities of specific features) --> 14 dictionaries
				- Conditional Probabilities --> 2 dictionaries, each with 14 dictionaries
"""
from collections import Counter

class Model:
	def __init__(self, training_data, testing_data):
		self.training_data = training_data
		self.testing_data = testing_data

		self.class_1_total = 0
		self.class_2_total = 0
		self.class_base_rates = {}
		self.feature_base_rates = {}
		self.conditional_probabilities = { '>50K' : {}, '<=50K' : {} }

		print("\tCalculating class base rates...")
		self.calculate_class_base_rates()
		print("\tCalculating feature base rates...")
		self.calculate_feature_base_rates()
		print("\tCalculating conditional probabilities...")
		self.calculate_all_conditional_probabilities()

		self.classified = []	# stores classifier results in parallel with each testing data record, for comparison
		self.confusion_matrix = { 'TP' : 0, 'TN' : 0, 'FP' : 0, 'FN' : 0 }
		
	def calculate_class_base_rates(self):
		base_rates = {}
		class_label_counter = Counter()

		for record in self.training_data:
			class_label_counter[record.get_class_label()] += 1

		self.class_1_total = class_label_counter['>50K']
		self.class_2_total = class_label_counter['<=50K']

		self.class_base_rates['>50K'] = class_label_counter['>50K'] / (class_label_counter['>50K'] + class_label_counter['<=50K'])
		self.class_base_rates['<=50K'] = class_label_counter['<=50K'] / (class_label_counter['>50K'] + class_label_counter['<=50K'])

	def calculate_attribute_frequencies(self, attribute):
		frequencies = {}
		attribute_counter = Counter()

		for record in self.training_data:
			attribute_counter[record.record_dict[attribute]] += 1

		total = 0
		for key in attribute_counter:
			total += attribute_counter[key]

		for key in attribute_counter:
			frequencies[key] = attribute_counter[key] / total

		self.feature_base_rates[attribute] = frequencies

	def calculate_feature_base_rates(self):
		self.calculate_attribute_frequencies('age')
		self.calculate_attribute_frequencies('workclass')
		self.calculate_attribute_frequencies('fnlwgt')
		self.calculate_attribute_frequencies('education')
		self.calculate_attribute_frequencies('education-num')
		self.calculate_attribute_frequencies('marital-status')
		self.calculate_attribute_frequencies('occupation')
		self.calculate_attribute_frequencies('relationship')
		self.calculate_attribute_frequencies('race')
		self.calculate_attribute_frequencies('sex')
		self.calculate_attribute_frequencies('capital-gain')
		self.calculate_attribute_frequencies('capital-loss')
		self.calculate_attribute_frequencies('hours-per-week')
		self.calculate_attribute_frequencies('native-country')

	def calculate_conditional_frequencies_of_attribute(self, attribute):
		class_1_frequencies = {}
		class_2_frequencies = {}

		class_1_attribute_counter = Counter()
		class_2_attribute_counter = Counter()

		for record in self.training_data:
			if record.get_class_label() == '>50K':
				class_1_attribute_counter[record.record_dict[attribute]] += 1
			else:
				class_2_attribute_counter[record.record_dict[attribute]] += 1

		for key in class_1_attribute_counter:
			class_1_frequencies[key] = class_1_attribute_counter[key] / self.class_1_total

		for key in class_2_attribute_counter:
			class_2_frequencies[key] = class_2_attribute_counter[key] / self.class_2_total

		(self.conditional_probabilities['>50K'])[attribute] = class_1_frequencies
		(self.conditional_probabilities['<=50K'])[attribute] = class_2_frequencies

	def calculate_all_conditional_probabilities(self):
		self.calculate_conditional_frequencies_of_attribute('age')
		self.calculate_conditional_frequencies_of_attribute('workclass')
		self.calculate_conditional_frequencies_of_attribute('fnlwgt')
		self.calculate_conditional_frequencies_of_attribute('education')
		self.calculate_conditional_frequencies_of_attribute('education-num')
		self.calculate_conditional_frequencies_of_attribute('marital-status')
		self.calculate_conditional_frequencies_of_attribute('occupation')
		self.calculate_conditional_frequencies_of_attribute('relationship')
		self.calculate_conditional_frequencies_of_attribute('race')
		self.calculate_conditional_frequencies_of_attribute('sex')
		self.calculate_conditional_frequencies_of_attribute('capital-gain')
		self.calculate_conditional_frequencies_of_attribute('capital-loss')
		self.calculate_conditional_frequencies_of_attribute('hours-per-week')
		self.calculate_conditional_frequencies_of_attribute('native-country')
