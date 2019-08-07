# 从GBDT到XGBoost

> 作业部落链接：https://www.zybuluo.com/HolyShine/note/1528516

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
即从之前样本的累积，变成了叶子节点的累积。

令 $\sum_{i\in I_j}g_i=G_j, \sum_{i\in I_j}h_i=H_j$， 则式(8) 可以改写为：
$$
\tag 9
Obj^{(t)}\simeq \sum_{j=1}^T \left[G_jw_j+\frac{1}{2}(H_j+\lambda)w_j^2\right]+\gamma T
$$
我们在第$t$ 步的目标就是要找一棵特定结构的树，使得上式最小。对于每一棵我们找到的候选树，结构是确定的(即 $w_q$ 确定)，也就是说哪些样本属于哪个叶子节点是固定的，即上述的 $G, H$ 固定。对于每一组固定的$G,H$，我们总能得到对应的 $w$，使得在这棵树的结构下，损失极小（事实上是最小，因为上述函数关于$w$ 凸）。也就是说我们可以固定$w$，显然即$\frac{\partial Obj}{\partial w^*_j}=0$：
$$
\tag {10}
w^*_j = -\frac{Gj}{H_j+\lambda}
$$
带回式(9)，得到：
$$
\tag {11}
Obj^{(t)}\simeq -\frac{1}{2}\sum_{j=1}^T\frac{G_j^2}{H_j+\lambda}+\gamma T
$$
现在我们的目标变为，在 $t$ 步寻找一棵特定结构的树，使得式(11)的损失尽可能小。

最朴素的想法是，枚举所有可能树的结构，计算式(11)的值，取最小的值对应的树的结构，但这么做计算量太大。   

XGBoost使用了贪心的策略，粗略的步骤如下：

1. 从深度为0的树开始，对每个叶节点枚举所有的可用特征
2. 针对每个特征，把属于该节点的训练样本根据该特征值升序排列，通过线性扫描的方式来决定该特征的最佳分裂点，并记录该特征的最大收益（采用最佳分裂点时的收益）
3. 选择收益最大的特征作为分裂特征，用该特征的最佳分裂点作为分裂位置，把该节点生长出左右两个新的叶节点，并为每个新节点关联对应的样本集
4. 回到第1步，递归执行到满足特定条件为止

那么如何计算第2步的收益呢？还是用式(11) 来做。

对于某一个节点，假设其可以分裂为 $L$ 和 $R$ 两个叶节点，其分裂前的损失为 $-\frac{1}{2}\frac{(G_L+G_R)^2}{H_L+H_R+\lambda}+\gamma $， 分裂后的损失为$-\frac{1}{2}\left[\frac{G_L^2}{H_L+\lambda}+\frac{G_R^2}{H_R+\lambda}\right]+2\gamma $，其收益为：
$$
\tag {12}
Gain = \frac{1}{2}\left[\frac{G_L^2}{H_L+\lambda}+\frac{G_R^2}{H_R+\lambda}-\frac{(G_L+G_R)^2}{H_L+H_R+\lambda}\right]-\gamma
$$
根据式(12) 即可判断使用哪个特征分裂获得的收益更高。

> **注意**，这个过程可以并行化优化。因为对于某个节点，选择特征是独立进行的，可以并行获得结果并比较收益

因此整个XGBoost的流程大致如下：

1. 算法在拟合的每一步都新生成一颗决策树；
2. 在拟合这棵树之前，需要计算损失函数在每个样本上的一阶导和二阶导，即 $g_i$ 和 $h_i$ (用于后续的 $G，H$ 的计算)；
3. 通过上面的贪心策略生成一颗树，计算每个叶子结点的$G_j$ 和$H_j$，再用式(10) 计算每个叶子节点的值 $w_j$；
4. 把新生成的决策树$f_t$ 加入 $\hat y^t_i=\hat y_i^{t-1}+\epsilon f_t(x_i)$ ，其中$\epsilon$ 为学习率，主要为了抑制模型的过拟合。

## 4. 参考资料

[知乎：机器学习-一文理解GBDT的原理-20171001](https://zhuanlan.zhihu.com/p/29765582)



  

