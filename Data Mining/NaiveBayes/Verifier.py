import NaiveBayes

class Verifier:
	def __init__(self, models, runtimes):
		self.models = models
		self.runtimes = runtimes

		self.compute_all_confusion_matrices()

		self.precision_by_model = self.compute_precision_by_model()
		self.recall_by_model = self.compute_recall_by_model()
		self.F1_by_model = self.compute_F1_by_model()
		self.accuracy_by_model = self.compute_accuracy_by_model()

	def construct_and_write_results(self):
		screen_o = ""

		screen_o += "\n----------------------------\nAggregate evaluation results\n----------------------------\n"
		screen_o += ("Average Micro Precision: " + str(round(self.compute_avg_micro_precision(), 2)) + '\n')
		screen_o += ("Average Micro Recall: " + str(round(self.compute_avg_micro_recall(), 2)) + '\n')
		screen_o += ("Average Micro F1: " + str(round(self.compute_avg_micro_precision(), 2)) + '\n')
		screen_o += ("Average Macro Precision: " + str(round(self.compute_macro_precision(), 2)) + '\n')
		screen_o += ("Average Macro Recall: " + str(round(self.compute_macro_recall(), 2)) + '\n')
		screen_o += ("Average Macro F1: " + str(round(self.compute_macro_F1(), 2)) + '\n')
		screen_o += ("Average Accuracy: " + str(round(self.compute_avg_accuracy(), 2)) + '\n')

		file_o = ""
		file_o += "\n-------------------\nRuntime information\n-------------------\n"
		for i in range(len(self.models)):
			file_o += ("Fold " + str(i + 1) + ": " + str(round(self.runtimes[i], 2)) + " seconds\n")

		for i in range(len(self.models)):
			file_o += ("\n-------------------------------\n" + "Evaluation results for Model " + str(i + 1) \
		 		+ "\n-------------------------------\n")
			file_o += ("Micro/Micro precision: " + str(round(self.precision_by_model[i], 2)) + '\n')
			file_o += ("Micro/Micro recall: " + str(round(self.recall_by_model[i], 2)) + '\n')
			file_o += ("Micro/Micro F1: " + str(round(self.F1_by_model[i], 2)) + '\n')
			file_o += ("Micro/Micro accuracy: " + str(round(self.accuracy_by_model[i], 2)) + '\n')

		file_o += screen_o

		print(screen_o)

		output_file = open('adult.out', 'w')
		output_file.write(file_o)
		output_file.close()

		print("Runtime information and results written to 'adult.out'")

	def calculate_confusion_matrix(self, model):
		for i in range(len(model.testing_data)):
			if model.classified[i] == '>50K' and model.testing_data[i].get_class_label() == '>50K':
				model.confusion_matrix['TP'] += 1

			if model.classified[i] == '>50K' and model.testing_data[i].get_class_label() == '<=50K': 
				model.confusion_matrix['FP'] += 1

			if model.classified[i] == '<=50K' and model.testing_data[i].get_class_label() == '>50K':
				model.confusion_matrix['FN'] += 1

			if model.classified[i] == '<=50K' and model.testing_data[i].get_class_label() == '<=50K':
				model.confusion_matrix['TN'] += 1


	def compute_all_confusion_matrices(self):
		for i in range(len(self.models)):
			self.calculate_confusion_matrix(self.models[i])

	# for an individual model, macro precision and micro precision are identical
	def calculate_precision(self, model):
		return model.confusion_matrix['TP'] / (model.confusion_matrix['TP'] + model.confusion_matrix['FP'])

	def compute_precision_by_model(self):
		precision_list = []

		for i in range(len(self.models)):
			precision_list.append(self.calculate_precision(self.models[i]))

		return precision_list

	# for an individual model, macro recall and micro recall are identical
	def calculate_recall(self, model):
		return model.confusion_matrix['TP'] / (model.confusion_matrix['TP'] + model.confusion_matrix['FN'])

	def compute_recall_by_model(self):
		recall_list = []

		for i in range(len(self.models)):
			recall_list.append(self.calculate_recall(self.models[i]))

		return recall_list

	# for an individual model, macro F1 and micro F1 are identical
	def calculate_F1(self, model):
		return (2 * self.calculate_recall(model) * self.calculate_precision(model)) / \
			(self.calculate_recall(model) + self.calculate_precision(model))

	def compute_F1_by_model(self):
		F1_list = []

		for i in range(len(self.models)):
			F1_list.append(self.calculate_F1(self.models[i]))

		return F1_list

	def calculate_accuracy(self, model):
		return (model.confusion_matrix['TP'] + model.confusion_matrix['TN']) / (model.confusion_matrix['TP'] + \
			model.confusion_matrix['TN'] + model.confusion_matrix['FP'] + model.confusion_matrix['FN'])

	def compute_accuracy_by_model(self):
		accuracy_list = []

		for i in range(len(self.models)):
			accuracy_list.append(self.calculate_accuracy(self.models[i]))

		return accuracy_list		

	def compute_avg_micro_precision(self):
		numerator = 0
		denominator = 0

		for i in range(len(self.models)):
			numerator += self.models[i].confusion_matrix['TP']
			denominator += (self.models[i].confusion_matrix['TP'] + self.models[i].confusion_matrix['FP'])

		return numerator / denominator


	def compute_avg_micro_recall(self):
		numerator = 0
		denominator = 0

		for i in range(len(self.models)):
			numerator += self.models[i].confusion_matrix['TP']
			denominator += (self.models[i].confusion_matrix['TP'] + self.models[i].confusion_matrix['FN'])

		return numerator / denominator

	def compute_avg_micro_F1(self):
		numerator = 0
		denominator = 0

		for i in range(len(self.models)):
			numerator += (2 * self.calculate_recall(model) * self.calculate_precision(model))
			denominator += (self.calculate_recall(model) + self.calculate_precision(model))

		return numerator / denominator

	def compute_macro_precision(self):
		total_precision = 0

		for i in range(len(self.models)):
			total_precision += self.precision_by_model[i]

		return total_precision / (i + 1)

	def compute_macro_recall(self):
		total_recall = 0

		for i in range(len(self.models)):
			total_recall += self.recall_by_model[i]

		return total_recall / (i + 1)

	def compute_macro_F1(self):
		total_F1 = 0

		for i in range(len(self.models)):
			total_F1 += self.F1_by_model[i]

		return total_F1 / (i + 1)

	def compute_avg_accuracy(self):
		numerator = 0
		denominator = 0

		for i in range(len(self.models)):
			numerator += (self.models[i].confusion_matrix['TP'] + self.models[i].confusion_matrix['TN'])
			denominator += (self.models[i].confusion_matrix['TP'] + self.models[i].confusion_matrix['TN'] + \
				self.models[i].confusion_matrix['FP'] + self.models[i].confusion_matrix['FN'])

		return numerator / denominator

