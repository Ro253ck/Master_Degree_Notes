import torch
import numpy as np

#Generate synthetic data
X=torch.linspace(0,1,100).reshape(-1,1)
y=3*X+2+0.1*torch.randn(X.size())

#Initialize weights and bias
w=torch.randn(1,requires_grad=True)
b=torch.randn(1,requires_grad=True)

#Define the Forward pass
def forward(X):
    return w*X+b

def mse_loss(y_pred,y):
    return torch.mean((y-y_pred)**2)

#Training loop
learning_rate=0.1
for epoch in range(100):
    predictions=forward(X)
    loss=mse_loss(predictions,y)
    loss.backward()

    with torch.no_grad():
        w-=learning_rate*w.grad
        b-=learning_rate*b.grad
        #zero gradients
        w.grad.zero_()
        b.grad.zero_()