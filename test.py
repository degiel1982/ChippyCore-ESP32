import numpy as np

# Generate training set for decimal to binary conversion
decimal_inputs = np.array([[i] for i in range(16)])
binary_outputs = np.array([[int(x) for x in f"{i:04b}"] for i in range(16)])  # 4-bit binary

# Initialize weights, biases, learning rate, and epoch count
np.random.seed(0)  # For reproducibility
weights1 = np.random.randn(1, 16) * np.sqrt(2 / 1)  # He initialization for first hidden layer
biases1 = np.zeros(16)
weights2 = np.random.randn(16, 16) * np.sqrt(2 / 16)  # He initialization for second hidden layer
biases2 = np.zeros(16)
weights3 = np.random.randn(16, 16) * np.sqrt(2 / 16)  # He initialization for third hidden layer
biases3 = np.zeros(16)
weights4 = np.random.randn(16, 16) * np.sqrt(2 / 16)  # He initialization for output layer
biases4 = np.zeros(16)
learning_rate = 0.001  # Smaller learning rate
epochs = 200000  # Increased epochs

# Activation functions
def relu(x):
    return np.maximum(0, x)

def softmax(x):
    exp_x = np.exp(x - np.max(x, axis=1, keepdims=True))
    return exp_x / exp_x.sum(axis=1, keepdims=True)

# Derivatives for backpropagation
def relu_derivative(x):
    return np.where(x > 0, 1, 0)

# Forward pass: calculate the outputs with ReLU and softmax
def forward_pass(inputs, weights1, biases1, weights2, biases2, weights3, biases3, weights4, biases4):
    hidden_layer1 = relu(np.dot(inputs, weights1) + biases1)
    hidden_layer2 = relu(np.dot(hidden_layer1, weights2) + biases2)
    hidden_layer3 = relu(np.dot(hidden_layer2, weights3) + biases3)
    output_layer = softmax(np.dot(hidden_layer3, weights4) + biases4)
    return hidden_layer1, hidden_layer2, hidden_layer3, output_layer

# Error function: cross-entropy loss
def cross_entropy_loss(predictions, targets):
    return -np.sum(targets * np.log(predictions + 1e-9)) / targets.shape[0]

# Backward pass: calculate the gradients
def backward_pass(inputs, hidden_layer1, hidden_layer2, hidden_layer3, output_layer, targets, weights2, weights3, weights4):
    output_error = output_layer - targets
    hidden_error3 = np.dot(output_error, weights4.T) * relu_derivative(hidden_layer3)
    hidden_error2 = np.dot(hidden_error3, weights3.T) * relu_derivative(hidden_layer2)
    hidden_error1 = np.dot(hidden_error2, weights2.T) * relu_derivative(hidden_layer1)
    weights4_gradient = np.dot(hidden_layer3.T, output_error)
    biases4_gradient = np.sum(output_error, axis=0)
    weights3_gradient = np.dot(hidden_layer2.T, hidden_error3)
    biases3_gradient = np.sum(hidden_error3, axis=0)
    weights2_gradient = np.dot(hidden_layer1.T, hidden_error2)
    biases2_gradient = np.sum(hidden_error2, axis=0)
    weights1_gradient = np.dot(inputs.T, hidden_error1)
    biases1_gradient = np.sum(hidden_error1, axis=0)
    return weights1_gradient, biases1_gradient, weights2_gradient, biases2_gradient, weights3_gradient, biases3_gradient, weights4_gradient, biases4_gradient

# Convert binary outputs to one-hot encoding for softmax
one_hot_outputs = np.zeros((binary_outputs.shape[0], 16))
for i in range(binary_outputs.shape[0]):
    one_hot_outputs[i][int("".join(map(str, binary_outputs[i])), 2)] = 1

# Training loop
for epoch in range(epochs):
    # Forward pass
    hidden_layer1, hidden_layer2, hidden_layer3, output_layer = forward_pass(decimal_inputs, weights1, biases1, weights2, biases2, weights3, biases3, weights4, biases4)
    
    # Compute the loss
    loss = cross_entropy_loss(output_layer, one_hot_outputs)
    
    # Backward pass
    gradients = backward_pass(decimal_inputs, hidden_layer1, hidden_layer2, hidden_layer3, output_layer, one_hot_outputs, weights2, weights3, weights4)
    weights1_gradient, biases1_gradient, weights2_gradient, biases2_gradient, weights3_gradient, biases3_gradient, weights4_gradient, biases4_gradient = gradients
    
    # Update the weights and biases
    weights1 -= learning_rate * weights1_gradient
    biases1 -= learning_rate * biases1_gradient
    weights2 -= learning_rate * weights2_gradient
    biases2 -= learning_rate * biases2_gradient
    weights3 -= learning_rate * weights3_gradient
    biases3 -= learning_rate * biases3_gradient
    weights4 -= learning_rate * weights4_gradient
    biases4 -= learning_rate * biases4_gradient
    
    # Print loss every 10000 epochs
    if epoch % 10000 == 0:
        print(f'Epoch {epoch}, Loss: {loss:.4f}')

# Final predictions
_, _, _, final_predictions = forward_pass(decimal_inputs, weights1, biases1, weights2, biases2, weights3, biases3, weights4, biases4)
binary_predictions = np.argmax(final_predictions, axis=1)  # Convert softmax outputs to class labels

print("Final Outputs:")
for i, (inputs, prediction) in enumerate(zip(decimal_inputs, binary_predictions)):
    print(f"Input: {inputs[0]}, Predicted Outputs: {np.binary_repr(prediction, width=4)}")
