# 从GBDT到XGBoost

## 1. 引子

GBDT**使用损失函数的负梯度来拟合当前模型的残差**，为什么这么说呢？

我们知道GBDT是一种Boosting学习方法，每次使用一个新的基学习器加入到整体模型中。  

事实上这种说法是特指使用均方误差损失函数的GBDT，假设整体模型为 $F(x)$, 则损失函数为：
$$
\tag 1
L(y, F(x))=\frac{1}{2}(y-F(x))^2
$$
损失函数的一阶导为：
$$
\tag 2
\frac{\partial L}{\partial F(x_i)} = F(x)-y = -(y-F(x))
$$
可以看到刚好残差就是负梯度。

>绝对值误差对于异常点过于敏感，因此回归模型多用**Absolute loss** 或者 **Huber loss**:
>
>Absolute loss:
>$$
>L(y,F(x)) = \vert y -F(x)\vert
>$$
>Huber loss:
>$$
>L(y,F(x))=\left \{
>\begin{array}{}
>&\frac{1}{2}(y-F(x))^2\quad\quad &\vert y -F(x)\vert\leq \delta \\
>&\delta(\vert y -F(x)\vert-\delta/2)&\vert y -F(x)\vert>\delta
>\end{array}
>\right.
>$$



## 2. Boosting加法模型

GBDT模型可以看做 $K$ 个基模型组成的复合模型：
$$
\tag 3
\hat y_i =\sum_{k=1}^K f_k(x_i),\quad f_k\in \mathcal{F}
$$
其中 $\mathcal{F}$ 是函数空间。

根据GBDT的训练过程，GBDT在每一个时间步根据当前整体模型的损失训练一个新的基学习器，加入到当前模型中。假设训练第 $t$ 个基学习器，则当前损失为（含正则）：
$$
\tag 4
Obj^{(t)}=\sum_{i=1}^Nl(y_i,\hat y_i^{t-1}+f_t(x_i))+\Omega(f_t(x_i))+constant
$$
事实上，GBDT的目标函数只包含上式的第一项，它对损失进行一阶泰勒展开。而XGBoost则在此基础上对损失函数二阶展开，并在目标函数里加上正则化。

## 3. XGBoost

对于目标函数 (4)， 将 $\hat y_i^{(t-1)}$ 视作 $x$， $f_t(x_i)$ 视作 $\Delta x$，根据泰勒二阶展开：
$$
\tag 5
f(x+\Delta x)\simeq f(x)+f'(x)\Delta x +\frac{1}{2}f''(x)\Delta x^2
$$
得到当前损失的展开：
$$
\tag 6
Obj^{(t)} \simeq \sum_{i=1}^N \left [l(y_i,\hat y_i^{t-1})+g_i f_t(x_i) +\frac{1}{2}h_if_t^2(x_i)\right ]+\Omega(f_t(x_i))+constant
$$
其中， $g_i=\frac{\partial(y_i,\hat y_i^{t-1})}{\partial \hat y_i^{t-1}}, h_i=\frac{\partial(y_i,\hat y_i^{t-1})}{\partial \hat y_i^{t-1}\partial \hat y_i^{t-1}}$ 分别表示当前整体模型损失（未整合 $f_t $）对 $\hat y_i^{t-1}$ 的一阶导和二阶导

在第 $t$ 步，$l(y_i, \hat y_i^{t-1})$ 是一个已知的值，所以不影响优化目标，因此优化目标可以写成：
$$
\tag 7
Obj^{(t)} \simeq \sum_{i=1}^N \left [g_i f_t(x_i) +\frac{1}{2}h_if_t^2(x_i)\right ]+\Omega(f_t(x_i))
$$
同样，$g_i$ 和 $h_i$ 也是确定好的值。因此在第 $t$ 步，只需要计算出上述确定好的值，再带入 (7) 式，就可以得到最终的优化目标。通过计算优化目标可以得到第 $t$ 步的学习器。  

> 可以看到，这个学习过程与一般的梯度下降存在区别：
>
> **一般的梯度下降优化的是参数空间，而 式 (7) 优化的是整个函数空间。**

现在还有一个问题，**如何表示这个基学习器$f_t$？**

我们知道XGBoost用的最多的是决策树里的CART树。我们用一个函数 $w_{q}$ 表示当前这棵决策树的输出，其中 $q(x_i)$ 表示将 $x_i$ 归为叶子节点 $q(x_i)$ ，假设有 $T$ 个叶子节点，则 $q(\cdot)\in \{1,2,3,\cdots,T\}$，$w_q$ 表示对于当前这棵树，节点 $q$ 的值。

用 $I_j =\{i|q(x_i)=j\} $ 表示第 $j$ 个叶子节点里的样本集合，则式(7) 可以做一些变换：
$$
\tag 8
\begin{array}{}
Obj^{(t)}&\simeq \sum_{i=1}^N \left [g_i f_t(x_i) +\frac{1}{2}h_if_t^2(x_i)\right ]+\Omega(f_t(x_i))\\
&=\sum_{i=1}^N \left [ g_iw_{q(x_i)} + \frac{1}{2}h_iw^2_{q(x_i)}\right]+\gamma T+\frac{1}{2}\lambda\sum_{i=1}^Tw_j^2\\
&=\sum_{j=1}^T\left[(\sum_{i\in I_j}g_i)w_j+\frac{1}{2}(\sum_{i\in I_j}h_i+\lambda)w_j^2\right]+\gamma T
\end{array}
$$
即从之前样本的累积，变成了叶子节点的累积