## What's this REPO

This repository forms part of the artifact for my published paper, [Panther](https://ieeexplore.ieee.org/document/10824864/).

Given a large integer $q$, we select a random secret key $s \in \mathbb{Z}_q[X]/(X^N+1)$, where the coefficients of $s$ can only take values from the set $\{1, -1, 0\}$. The multiplicative inverse of the secret key is denoted by $s^{-1} \in \mathbb{Z}_q[X]/(X^N+1)$.

Our core assumption is as follows:
- The infinity norm $||s^{-1}||_\infty$ is approximately $\frac{q}{2}$.
- The average value of the coefficients of $s^{-1}$ tends to be close to $\frac{q}{4}$.

> **Note:**  
> This is because the ring $\mathbb{Z}_q = [0, q) \cap \mathbb{Z}$ and we use $\mathbb{Z}_q = [q/2, q)$ to represent negative integers. Therefore, the largest value of $s^{-1}$ is $\frac{q}{2}$.

## How to Build This Project

This project was developed on **Ubuntu**, but any Linux distribution should work.

### Step 1: Install Dependencies

On Ubuntu, install the GMP library (used for large number arithmetic in the inverse RNS transformation) by running:

```bash
apt-get install libgmp-dev
```
### Step 2: Clone and Build the Project
Clone the repository and build the project using the following commands:
```bash
git clone https://github.com/YefanWu/SEAL-SECRETKEY-VIEW.git

cd SEAL-SECRETKEY-VIEW
mkdir build
cd build
cmake ..
make
```
### How to Run This Project
After building the project, execute the following command from the root directory of the repository:
```bash
build/bin/secretkey_view
```