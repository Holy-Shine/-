## 1. 硬间隔SVM

首先回顾下我们的老朋友分类超平面：
$$
\tag 1
w^T x+b = 0
$$
定义函数间隔：
$$
\tag 2
\hat \gamma = yf(x)=y(w^Tx+b)
$$
以及几何间隔(点到超平面距离)：
$$
\tag 3
\tilde \gamma = \frac{w^Tx+b}{\parallel w\parallel}
$$
对于函数间隔来说，我们在超平面固定后，可以等比例缩放 $w$ 的长度和 $b$ 的值，使得函数间隔任意大，因此几何间隔更适合作为整个margin。

由上述可知两种间隔的关系为 $\hat \gamma =\tilde \gamma \parallel w\parallel$。**对于确定好的超平面，有任意的 $(\hat \gamma, w)$ 对，满足这种关系**，因此不妨令 $\hat \gamma =1$，则得到了硬间隔SVM目标函数基本型：
$$
\tag 4
\max \frac{1}{\parallel w\parallel}\quad s.t.\ y_i(w^T x_i +b)\geq 1, i=1,\cdots, n
$$
等价于：
$$
\tag 5
\min\frac{1}{2} \parallel w\parallel^2 \quad s.t.\ y_i(w^T x_i +b)\geq 1, i=1,\cdots, n
$$
这是一个二次优化问题---目标函数是二次的，约束条件是线性的，可以通过QP优化包求解。但是这个问题其实有特殊结构，可以用拉格朗日对偶问题更有效地求解，此外，在推导过程中，许多有趣的特征也会被揭露出来。

### 1.1 拉格朗日转化为对偶问题

对不等式约束加入拉格朗日乘子，转化为无约束问题：
$$
\tag 6
L(w,b,\alpha) = \frac{1}{2} \parallel w\parallel^2-\sum_{i=1}^n\alpha_i(y_i(w^Tx_i+b)-1)
$$
我们令：
$$
\tag 7
\theta(w)= \max\limits_{\alpha_i\geq 0}L(w,b,\alpha)
$$
容易证明，当某个约束条件不满足时， 例如 $y_i(w^Tx_i+b)<1$，那么我们只要令 $\alpha = \infty$ ，则 $\theta(w)=\infty$ ，而当约束条件均满足的时候，有 $\theta(w)=\frac{1}{2}\parallel w\parallel^2$，即：
$$
\tag 8
\min\limits_{w,b}\theta(w) = \min\limits_{w,b}\max\limits_{\alpha_i\geq 0}L(w,b,\alpha )
$$
在满足 KKT 条件的情况下， min和max可交换，新的目标函数转化为：
$$
\tag 9
\max\limits_{\alpha_i\geq 0}\min\limits_{w,b}L(w,b,\alpha )
$$
对于内层求极小值：
$$
\tag {10}
\begin{align}
\frac{\partial L}{\partial w}  = 0&\Rightarrow w=\sum_{i=1}^n \alpha_iy_ix_i\\
\frac{\partial L}{\partial b}  = 0&\Rightarrow \sum_{i=1}^n \alpha_iy_i=0
\end{align}
$$
带回$L$:
$$
\tag {11}
\begin{align}
L(w,b,\alpha) &= \frac{1}{2}\left(\sum_{i=1}^n\alpha_iy_ix_i\right)^2+\sum_{j=1}^n\alpha_j \left(1-y_j\left(\sum_{i=1}^n\alpha_iy_ix_i^Tx_j+b\right)\right)\\
&=\sum_{i=1}^n\alpha_i-\frac{1}{2}\sum_{i=1}^n\sum_{j=1}^n\alpha_i\alpha_jy_iy_jx_i^Tx_j
\end{align}
$$
此时原问题的对偶优化为题为：
$$
\tag {12}
\max\limits_\alpha \sum_{i=1}^n\alpha_i-\frac{1}{2}\sum_{i=1}^n\sum_{j=1}^n\alpha_i\alpha_jy_iy_jx_i^Tx_j\\ s.t. \alpha_i \geq 0 \quad \text{and}\quad \sum_{i=0}^n \alpha_iy_i=0
$$
### 1.2 KKT条件

上述对偶问题的转化必须遵循KKT条件，即：
$$
\left \{ 
\begin{array}{l}
\alpha_i \geq 0\\
y_i(w^Tx_i+b)\geq 1\\
\alpha_i(y_i(w^Tx_i+b)-1)=0
\end{array}
\right.
$$

从KKT条件可知，非支持向量 $\alpha$ 值为0，即只有支持向量参与训练。

这里顺便回答一个问题：

> **为什么SVM要转化为对偶问题?**
>
> - 带约束的原问题求解困难，变成对偶问题可以将约束放到一个表达式中
> - 对偶问题自然引入了核技巧，方便了后续解决低维线性不可分问题。



## 2. 软间隔SVM

数据有噪声outlier对超平面的影响较大，因此引入松弛变量 $\xi_i \geq 0$ ，对应**数据点 $x_i$ 允许偏离的函数间隔的量**，即约束条件变为：
$$
\tag {13}
y_i(w^Tx_i+b)\geq 1-\xi_i
$$
但是当 $\xi_i$ 任意大的话，所有超平面都符合条件了，因此需要加上约束，使得它们的总和也要小，故原来的目标函数变为：
$$
\tag {14}
\min \frac{1}{2}\parallel w\parallel^2+C\sum_{i=1}^n \xi_i\\
s.t. \quad y_i(w^Tx_i+b)\geq 1-\xi_i\\
\xi_i\geq 0
$$
$C$ 是一个权衡参数，用于权衡**寻找margin最大的超平面**和**保证数据点偏差量最小**之间的权重。  

带入硬间隔的目标函数中，得到如下问题：
$$
\tag {15}
L(w,b,\xi,\alpha,r) = \frac{1}{2} \parallel w\parallel^2+C\sum_{i=1}^n\xi_i-\\\sum_{i=1}^n\alpha_i(y_i(w^Tx_i+b)-1+\xi_i)-\sum_{i=1}^nr_i\xi_i
$$
同样先让 $L$ 对 $w,b,\xi$ 最小化，得到:
$$
\tag {16}
\begin{align}
w=\sum_{i=1}^n \alpha_iy_ix_i\\
\sum_{i=1}^n\alpha_i y_i=0\\
C-\alpha_i-r_i = 0
\end{align}
$$
带回 $L$ 化简，得到了和硬间隔SVM一样的目标函数：
$$
\tag {17}
\max_{\alpha} \sum_{i=1}^n \alpha_i -\frac{1}{2}\sum_{i,j=1}^n \alpha_i\alpha_j y_iy_j\langle x_i, x_j\rangle
$$
因为 $r_i\geq0$，有 $\alpha_i \leq C$，因此整个对偶问题写作：
$$
\tag {18}
\max_{\alpha} \sum_{i=1}^n \alpha_i -\frac{1}{2}\sum_{i,j=1}^n \alpha_i\alpha_j y_iy_j\langle x_i, x_j\rangle\\
s.t. 0\leq \alpha_i \leq C, i=1,\cdots, n\\
\sum_{i=1}^n \alpha_i y_i=0
$$
我们发现软间隔优化的目标和硬间隔优化目标仅有一个，**$\alpha_i$ 多了一个上限 $C$**。

通过KKT条件，有如下约束：
$$
\tag {19}
\left\{
\begin{array}{}
\alpha_i\left[y_i(w^Tx_i +b)-1+\xi_i\right]=0\\
r_i\xi_i = 0\\
\xi_i \geq 0 
\end{array}
\right.
$$
对于 $\alpha_i > 0$ 的点，必须满足 $y_i(w^Tx_i+b)=1-\xi_i$：

- 若 $\alpha_i<C$，则 $r_i>0, \xi_i=0$，则该点位于支持平面上
- 若 $\alpha_i =C$，则$r_i=0$，此时若$\xi_i\leq 1$，那么该点位于支持平面里，若 $\xi_i\geq1$，则是被误分的点。

可以看到加入了松弛变量后，整个目标函数对数据点增加了容忍度。

## 3. 核函数

核函数的应用来自于数学的证明：

> 如果原始空间是有限维，则一定存在一个高维使得数据线性可分

SVM的高维形式很简单，只需令 $\phi(x)$ 表示 $x$ 映射到高维空间的向量，之后对于SVM所有的推导，都只要令 $x=\phi(x)$即可。

但是这里有个问题，求解 $\phi(x_i)^T\phi(x_j)$ 不易，所以定义核函数：
$$
\tag {20}
K(x_i,x_j)=\langle \phi(x_i),\phi(x_j)\rangle
$$
即，**定义一个计算原始空间 $x_i,x_j$ 的函数 $K$ 来表示等价的高维空间內积**

据此得到核SVM的解的形式：
$$
\tag {20}
f(x)= w^T\phi(x)+b =\sum_{i=1}^n \alpha_iy_iK(x_i,x_j)+b
$$

### 3.1 常见的核函数

- 线性核：$K(x_i,x_j)=x_i^Tx_j$
- 多项式核：$K(x_i,x_j)=(x_i^Tx_j)^d$
- 高斯核(RBF)：$K(x_i,x_j)=\exp(-\frac{\parallel x_i-x_j\parallel^2}{2\sigma^2})$
- 拉普拉斯核：$K(x_i,x_j)=\exp(-\frac{\parallel x_i-x_j\parallel}{\sigma})$
- Sigmoid核：$K(x_i,x_j)=\tanh(\beta x_i^Tx_j+\theta)$