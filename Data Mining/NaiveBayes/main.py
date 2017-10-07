import Preprocessor
import NaiveBayes
import Verifier

preprocessor = Preprocessor.Preprocessor()
preprocessor.clean_data_and_export()

classifier = NaiveBayes.NaiveBayes()
classifier.build_models_and_classify_data()

verifier = Verifier.Verifier(classifier.models, classifier.runtimes)
verifier.construct_and_write_results()