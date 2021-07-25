## Cluster Maintenance

### OS upgrades

```yaml
# 해당 노드에 스케줄 불가능 하도록 
# 해당 노드에서 작동중인 pod 를 종료시키고, (replicaset 에 정의 된 경우) 다른 노드로 옮긴다. 
# safely evict all of your pods from a node. 
# Safe evictions allow the pod's containers to gracefully terminate and will respect the PodDisruptionBudgets you have specified.
# 따라서 중요한 어플리케이션을 담은 Pod 의 경우, deployment 로 감싸줘야한다. 
kubectl drain node-1

# 해당 노드에 작동중인 pod 를 종료시키지 않고, 추가적인 pod 할당을 불가능 하게 한다. 
kubectl cordon node-2

# 다시 해당 노드에 스케줄 가능하도록 
kubectl uncordon node-1
```



### Cluster upgrade

- Master Node



- Workder Node
  1. 모두 한번에 업그레이드
  2. 노드 한개씩 업그레이드
  3. 새로운 노드를 cluster에 추가한다. (노드를 추가로 넣었다가 빼기 편리한 상황)



**kubeadm** - upgrade

? master 노드에 있는 kubelet (조사하기)

The reason is that kubeadm uses containers (pods) to deploy etcd and the api server components. For this static manifests are created as yaml-files which are picked up by the kubelet on the master node to provide the infrastructure pods.

An added benefit is that you have the master node metrics available from the kubernetes api.



```yaml
# checking avaliable upgrade
kubeadm upgrade plan
```



**Control Plane Node Upgrade**

On the controlplane node, run the command run the following commands:

1. `apt update`
   This will update the package lists from the software repository.

   

2. `apt install kubeadm=1.20.0-00`
   This will install the kubeadm version 1.20

   

3. `kubeadm upgrade apply v1.20.0`
   This will upgrade kubernetes controlplane. Note that this can take a few minutes.

   

4. `apt install kubelet=1.20.0-00` This will update the kubelet with the version 1.20.

   

5. You may need to restart kubelet after it has been upgraded.
   Run: `systemctl restart kubelet`



**Worker Node Upgrade**

On the node01 node, run the command run the following commands:

1. If you are on the master node, run `ssh node01` to go to `node01`

   

2. `apt update`
   This will update the package lists from the software repository.

   

3. `apt install kubeadm=1.20.0-00`
   This will install the kubeadm version 1.20

   

4. `kubeadm upgrade node`
   This will upgrade the node01 configuration.

   

5. `apt install kubelet=1.20.0-00` This will update the kubelet with the version 1.20.

   

6. You may need to restart kubelet after it has been upgraded.
   Run: `systemctl restart kubelet`

   

7. Type `exit` or enter `CTL + d` to go back to the controlplane node.



### ETCD 백업 방법

1. etcd snapshot 을 남긴다. 
2. 원하는 위치에 restore 한다. 
3. static pod 위치에 있는 etcd.yaml 파일에서 변경된 위치의 etcd mount 정보를 입력한다. 
4. 만약 etcd pod 가. pending 인 경우, 해당 파드를 지워본다. 

~~~yaml
# check etcd version
kubectl logs etcd-controlplane -n kube-system
kubectl describe pod etcd-controlplane -n kube-system | grep Image

# store snapshot of etcd
ETCDCTL_API=3 etcdctl --endpoints=https://127.0.0.1:2379 \
  --cacert=<trusted-ca-file> --cert=<cert-file> --key=<key-file> \
  snapshot save <backup-file-location>


# restore the snapshot.db
ETCDCTL_API=3 etcdctl --endpoints 127.0.0.1:2379 snapshot restore /opt/snapshot-pre-boot.db
ETCDCTL_API=3 etcdctl  --data-dir /var/lib/etcd-from-backup \
snapshot restore /opt/snapshot-pre-boot.db
~~~

