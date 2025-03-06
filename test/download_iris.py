from ucimlrepo import fetch_ucirepo
from sklearn.model_selection import train_test_split

# fetch dataset 
print("Fetching Iris dataset...")
iris = fetch_ucirepo(id=53)

# data (as pandas dataframes) 
X = iris.data.features
y = iris.data.targets

min_values = X.min()
max_values = X.max()

print("Minimum values for each column:")
print(min_values)

print("Maximum values for each column:")
print(max_values)

# split data into training and testing sets
X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=0.2, random_state=42)

print("Training data:")
print(X_train[:5])
print(y_train[:5])

print("Testing data:")
print(X_test[:5])
print(y_test[:5])

# convert to JSON
X_train_json = X_train.to_json(orient='records', indent=4)
y_train_json = y_train.to_json(orient='records', indent=4)
X_test_json = X_test.to_json(orient='records', indent=4)
y_test_json = y_test.to_json(orient='records', indent=4)

# save to JSON files
with open('iris_features_train.json', 'w') as f:
    f.write(X_train_json)

with open('iris_targets_train.json', 'w') as f:
    f.write(y_train_json)

with open('iris_features_test.json', 'w') as f:
    f.write(X_test_json)

with open('iris_targets_test.json', 'w') as f:
    f.write(y_test_json)    