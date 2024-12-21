import pandas as pd
from sklearn.feature_extraction.text import TfidfVectorizer
from sklearn.model_selection import train_test_split
from sklearn.linear_model import LogisticRegression
from sklearn.metrics import classification_report

data = pd.read_csv("feedback_dataset.csv")

X = data['Feedback']
y = data['Sentiment']

X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=0.2, random_state=42)

vectorizer = TfidfVectorizer(max_features=500, stop_words='english')
X_train_vec = vectorizer.fit_transform(X_train)
X_test_vec = vectorizer.transform(X_test)

model = LogisticRegression()
model.fit(X_train_vec, y_train)

y_pred = model.predict(X_test)
print("Classification Report :\n", classification_report(y_test, y_pred))

def classify_feedback(feedback):
	 feedback_vec = vectorizer.transform([feedback])
	 sentiment = model.predict(feedback_vec)[0]
	 return sentiment
	
user_feedback = "The agent was extremely helpful and the parking was excellent."
print("Feedback Sentiment : ", classify_feedback(user_feedback))
