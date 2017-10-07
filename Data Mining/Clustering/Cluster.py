import Records
from collections import Counter
import math

class Cluster:
	"""
		docstring for Cluster
	"""

	def __init__(self, record):
		self.centroid = record
		self.entries = set((record,))

		self.num_totals = {'age': record.age(), 'fnlwgt': record.fnlwgt(), 'education-num': record.edu_num(), \
			'capital-gain': record.cap_gain(), 'capital-loss': record.cap_loss(), 'hours-per-week': record.hours_per_week()}

		self.cat_frequencies = {'workclass': Counter(), 'education': Counter(), 'marital-status': Counter(), \
			'occupation': Counter(), 'relationship': Counter(), 'race': Counter(), 'sex': Counter(), \
			'native-country': Counter()}

		for attribute in record.cat_attributes:
			self.cat_frequencies[attribute][record.cat_attributes[attribute]] += 1

		# self.class_label = record.class_label

	def __str__(self):
		print_string = ""

		print_string += "Cluster centroid: %s\n" % str(self.centroid)
		
		print_string += "Entries in cluster:\n"
		for entry in self.entries:
			print_string += '%s\n' % str(entry)

		print_string += "Numerical attribute totals for cluster:\n%s\n\n" % str(self.num_totals)

		print_string += "Categorical frequencies for cluster:\n"
		for attribute in self.cat_frequencies:
			print_string += "\t%s: %s\n" % (attribute, str(self.cat_frequencies[attribute]))

		return print_string

	def get_centroid(self):
		return self.centroid

	def get_entries(self):
		return self.entries

	def size(self):
		return len(self.entries)

	def euclidian_distance(self, obj1, obj2):
		distance = 0

		for attribute in self.num_totals:
			distance += pow(float(obj1.num_attributes[attribute]) - float(obj2.num_attributes[attribute]), 2)

		for attribute in self.cat_frequencies:
			if obj1.cat_attributes[attribute] == obj2.cat_attributes[attribute]:
				distance += 1
			else:
				distance += 0

		return math.sqrt(distance)

	# mostly for readability down the road, less confusion
	def centroid_distance(self, cluster):
		return self.euclidian_distance(self.centroid, cluster.get_centroid())

	def single_link_distance(self, cluster):
		minDistance = None

		for entry1 in self.entries:
			for entry2 in cluster.get_entries():
				distance = self.euclidian_distance(entry1, entry2)

				if minDistance == None:
					minDistance = distance

				elif distance < minDistance:
					minDistance = distance

		return minDistance

	def complete_link_distance(self, cluster):
		maxDistance = None

		for entry1 in self.entries:
			for entry2 in cluster.get_entries():
				distance = self.euclidian_distance(entry1, entry2)

				if maxDistance == None:
					maxDistance = distance

				elif distance > maxDistance:
					maxDistance = distance
					
		return maxDistance	

	def merge(self, cluster):
		for attribute in self.num_totals:
			self.num_totals[attribute] = str(float(self.num_totals[attribute]) + float(cluster.num_totals[attribute]))

		for attribute in self.cat_frequencies:
			for value in cluster.cat_frequencies[attribute]:
				if value not in self.cat_frequencies[attribute]:
					self.cat_frequencies[attribute][value] = cluster.cat_frequencies[attribute][value]
				else:
					self.cat_frequencies[attribute][value] += cluster.cat_frequencies[attribute][value]

		self.entries.update(cluster.get_entries())
		return self

	def update_centroid(self):
		for attribute in self.centroid.num_attributes:
			self.centroid.num_attributes[attribute] = str(float(self.num_totals[attribute]) / self.size())

		for attribute in self.centroid.cat_attributes:
			self.centroid.cat_attributes[attribute] = self.cat_frequencies[attribute].most_common(1)[0][0]

	def sum_squared_error(self):
		cluster_SSE = 0

		for entry in self.entries:		
			for attribute in self.centroid.num_attributes:
				cluster_SSE += pow(float(self.centroid.num_attributes[attribute]) - float(entry.num_attributes[attribute]), 2)

			for attribute in self.centroid.cat_attributes:
				if self.centroid.cat_attributes[attribute] == entry.cat_attributes[attribute]:
					cluster_SSE += 1
				else:
					cluster_SSE += 0

		return cluster_SSE

	def add_entry(self, record):
		for attribute in record.num_attributes:
			self.num_totals[attribute] = str(float(self.num_totals[attribute]) + float(record.num_attributes[attribute]))

		for attribute in record.cat_attributes:
			if record.cat_attributes[attribute] not in self.cat_frequencies[attribute]:
				self.cat_frequencies[attribute][record.cat_attributes[attribute]] = 1
			else:
				self.cat_frequencies[attribute][record.cat_attributes[attribute]] += 1

		self.entries.add(record)

	def precision(self):
		correct_class = 0

		for record in self.entries:
			if record.class_label == self.centroid.class_label:
				correct_class += 1

		return correct_class / len(self.entries)
