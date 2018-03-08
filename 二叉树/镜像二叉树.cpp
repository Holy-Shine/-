/*镜像二叉树
    8                8
   / \              / \
  6  10     ===>   10  6
 / \ / \          / \ / \
5  7 9 11        11 9 7  5

由底向上或由上向下都可以：
前者：
    1.对一个根节点，如果它有孩子，则交换两个孩子
    2.对两个孩子分别做镜像。
*/

#include <iostream>
#include <queue>
#define NODE   7
using namespace std;

struct BinaryTreeNode{
    int                 value;
    BinaryTreeNode      *m_pLeft;
    BinaryTreeNode      *m_pRight;
};


// 使用前一个重建二叉树创建二叉树
BinaryTreeNode* constructCore
            (int *preStart,int *preEnd,
             int *midStart,int *midEnd);
BinaryTreeNode* construct(int *pre, int *mid, int length){
    if(pre==NULL||mid==NULL||length<=0)
        return NULL;
    return constructCore(pre, pre+length-1, mid, mid+length-1);
}

BinaryTreeNode* constructCore
            (int *preStart,int *preEnd,
             int *midStart,int *midEnd){
    // 前序遍历的第一个值为root
    BinaryTreeNode *root = new BinaryTreeNode();
    root->value = *preStart;
    
    // 出口
    if(preStart==preEnd){
        // 只剩1个单节点
        if(midStart==midEnd && *midStart==*preStart)
            return root;
        else
            throw "Invalid input";
    }
    // 在中序遍历中找到根节点的位置
    int *rootNode = midStart;
    while(rootNode!=midEnd && *rootNode!=*preStart)
        rootNode++;
    if(rootNode==midEnd && *rootNode!=*preStart)
        throw "Invalid input";
    
    // 左子树位置
    int Leftlength = rootNode - midStart;
    int *leftpreEnd = preStart+Leftlength;
    if(Leftlength>0){
        // 构建左子树
        root->m_pLeft = constructCore(preStart+1, leftpreEnd, midStart, rootNode-1);
    }
    
    //构建右子树
    if(preEnd-preStart>Leftlength){
        root->m_pRight = constructCore(leftpreEnd+1, preEnd, rootNode+1, midEnd);
    }
    return root;
                  
}


// 输出二叉树
void printBinaryTree(BinaryTreeNode *root){
    if(root==NULL) return;
    cout<<root->value<<'\t';
    printBinaryTree(root->m_pLeft);
    printBinaryTree(root->m_pRight);
}

// 镜像二叉树
void mirrorTree(BinaryTreeNode *root){
    // 如果是叶节点，或者空
    if(root==NULL || root->m_pLeft==NULL && root->m_pRight==NULL) 
        return;
    // 交换
    BinaryTreeNode *temp = root->m_pLeft;
    root->m_pLeft  = root->m_pRight;
    root->m_pRight = temp;
    // 递归交换
    mirrorTree(root->m_pLeft);
    mirrorTree(root->m_pRight);
}
int main() {
	// your code goes here
    int pre[7]={8,6,5,7,10,9,11};
    int mid[7]={5,6,7,8,9,10,11};
    BinaryTreeNode *root = construct(pre,mid,7);
    printBinaryTree(root);
    mirrorTree(root);
    cout<<endl;
    printBinaryTree(root);
	return 0;
}