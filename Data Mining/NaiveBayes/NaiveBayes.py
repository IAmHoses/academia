import Records
import Model
from array import array
import time

import random
import copy
import math

class NaiveBayes:
	def __init__(self):
		print("Preparing classifier...")
		self.preprocessed_data = Records.load_preprocessed_data()
		self.randomized_records = self.randomize_records()
		self.models = []
		self.runtimes = array('f', [0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0])
		print('\tClassifier created!\n')

	def randomize_records(self):
		print("\tRandomizing data record indices for classifier instantiation...")
		list_of_indices = []

		for index in range(len(self.preprocessed_data)):
			list_of_indices.append(index)

		random.shuffle(list_of_indices)
		return(list_of_indices)
	
	def __str__(self):
		all_records = ""
		for record in self.preprocessed_data:
			all_records += (str(record) + '\n')
		return all_records
	
	def build_models_and_classify_data(self):
		print("Training data...\n")
		self.make_models()
		print("Classifying testing data...")
		self.classify_all_models()
		print("\nAll models successfully trained and tested!\n")

	def make_models(self, fold_factor = 0.1):
		starting_index = 0

		for fold_number in range(10):
			t_0 = time.time()

			print("Preparing fold " + str(fold_number + 1) + "...")
			training_data = copy.deepcopy(self.preprocessed_data)
			testing_data = []

			ending_index = math.floor((len(self.preprocessed_data) * fold_factor) - 1)

			print("Constructing Model " + str(fold_number + 1) + "...")
			for i in range(starting_index, ending_index):
				testing_data.append(training_data[self.randomized_records[i]])
				training_data.remove(training_data[self.randomized_records[i]])
				training_data.insert(self.randomized_records[i], 'null')

			for j in range(len(testing_data)):
				training_data.remove('null')

			new_model = Model.Model(training_data, testing_data)
			print("\tDone!\n")

			self.models.append(new_model)

			self.runtimes[fold_number] = time.time() - t_0

			fold_factor = round(fold_factor + 0.1, 2)
			fold_number += 1
			starting_index = ending_index + 1

		if len(self.models) == 10:
			print("10 randomized models successfully stored in classifier!\n")

	def predict_class_label(self, index_of_model, record):
		class_1_probability = 1
		class_2_probability = 1

		for attribute in record.record_dict:
			if attribute == 'class-label':
				continue
			
			elif record.get_class_label() == '>50':
				if record.record_dict[attribute] == 'Never-worked':
					continue
				elif record.record_dict[attribute] == 'Holand-Netherlands':
					continue
				elif record.record_dict[attribute] == 'Outlying-US(Guam-USVI-etc)':
					continue
				elif record.record_dict[attribute] == 'Preschool':
					continue
				elif record.record_dict[attribute] == 'Laos':
					class_1_probability += (2 / self.models[index_of_model].class_1_total) / \
						(2 / (self.models[index_of_model].class_1_total + self.models[index_of_model].class_2_total))

				else:
					class_1_probability *= ((self.models[index_of_model].conditional_probabilities['>50K'])[attribute])[record.record_dict[attribute]] / \
						(self.models[index_of_model].feature_base_rates[attribute])[record.record_dict[attribute]]

			else:
				if record.record_dict[attribute] == 'Holand-Netherlands':
					class_2_probability += (1 / self.models[index_of_model].class_2_total) / \
					(1 / (self.models[index_of_model].class_1_total + self.models[index_of_model].class_2_total))
				else:
					class_2_probability *= ((self.models[index_of_model].conditional_probabilities['<=50K'])[attribute])[record.record_dict[attribute]] / \
						(self.models[index_of_model].feature_base_rates[attribute])[record.record_dict[attribute]]

		class_1_probability = class_1_probability * self.models[index_of_model].class_base_rates['>50K']
		class_2_probability = class_2_probability * self.models[index_of_model].class_base_rates['<=50K']

		if class_1_probability > class_2_probability:
			self.models[index_of_model].classified.append('>50K')
		else:
			self.models[index_of_model].classified.append('<=50K')

	def classify_all_models(self):
		for i in range(len(self.models)):
			t_0 = time.time()

			print("\tModel " + str(i + 1) + "...")
			for record in self.models[i].testing_data:
				self.predict_class_label(i, record)
			print("\tClassification complete!")

			self.runtimes[i] += time.time() - t_0
