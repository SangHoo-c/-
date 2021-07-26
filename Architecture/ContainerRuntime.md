### OS 란?

운영 체제는 실행되는 응용 프로그램들이 메모리와 [CPU](https://ko.wikipedia.org/wiki/중앙_처리_장치), 입출력 장치 등의 자원들을 사용할 수 있도록 만들어 주고, 이들을 [추상화](https://ko.wikipedia.org/wiki/추상화)하여 [파일 시스템](https://ko.wikipedia.org/wiki/파일_시스템) 등의 서비스를 제공한다. 또한 [멀티태스킹](https://ko.wikipedia.org/wiki/멀티태스킹)을 지원하는 경우, 여러 개의 응용 프로그램을 실행하고 있는 동안, 운영 체제는 이러한 모든 프로세스들을 스케줄링하여 마치 그들이 동시에 수행되는 것처럼 보이는 효과를 낸다.



### Thread Safe 란?





### Kernel 

- chroot : 프로세스에 대하여 새로운 root 디렉토리를 지정하는 것 

  Ex) 새로운 root 디렉토리가 지정된 프로세스는 할당된 root 디렉토리 하위로만 접근 가능 

  **프로세스에 대한 파일시스템 격리** but, 시스템 설정 및 환경은 동일 

- namesapce : 프로세스 별 리소스 사용을 분리하는 것 
  Ex) namespace a 에서 동작하는 aa 프로세스에서 hostname 을 변경한다해도 
  namespace b 에서 동작하는 bb 프로세스의 hostaname 은 변하지 않음. 
  **프로세스에 대한 환경 격리** but, 하드웨어 자원에 대한 분리는 수행하지 않음 

- cgroup : 프로세스 별 ( Process Group 단위로 ) 가용 컴퓨팅 자원 제한 & 격리 (메모리, cpu, device, disk i/o)
  Ex) cpu 사용량 나눈다... 
  Container 의 Resource 제어를 위해 사용  

- netlink

- Netfilter







### Container Runtime 

**Q1. 도커 자체가 컨테이너 런타임인가?**

**Q2. kubelet 은 Docker 와 어떤 방식으로 상호작용하여 컨테이너를 실행하는가?**

리눅스 컨테이너 기술을 쉽게 다루기 위해 도커 개발 
=> 컨테이너 기술은 OCI 에서 정한 런타임 & 이미지 명세를 준수하며 발전 
=> k8s 등에서 컨테이너 런타임에 접근하기 위한 표준 스펙인 CRI 가 공개 



**A1. Docker 는 컨테이너 런타임**

컨테이너 실행 순서는 이와 같다. 

1. 컨테이너 이미지 다운로드
2. 이미지 압축해체, 컨테이너 파일시스템 'Bundle' 생성 
3. Bundle 로 부터 컨테이너 생성 

Docker 는 3단계를 runC 라는 툴로 OCI 에 제공 



* Low Level Container Runtime 

순서 3에 해당, 컨테이너를 실행하는 기능만 제공, OCI runtime,

Namespace (FileSystem, Network 시스템 리소스 의 가상화 및 격리 )
Cgroups (컨테이너 안에서 사용할 수 있는 리소스 양 제한)

namespace 와 cgroup 의 설정 및 실행을 담당한다. 

현재 OCI 표준에는 runC. 


* High Level Container Runtime

이미지 관리, 압축해제 의 기능 

보통은 데몬 방식으로 동작, Remote API 를 제공, 외부에서 컨테이너를 실행하거나 모니터링 가능 

Containerd : runC 를 제어하는 데몬, 전체 컨테이너 life cycle( 이미지 전송 & 저장, 컨테이너 실행 & 관리, low-level 저장 & 네트워크 첨부) 관리 가능 



**A2. CRI 를 통해 kubelet이 컨테이너 런타임을 호출 할 수 있다.**

- High level container runtime 과 k8s 가 연계되기 위해선 CRI 가 필수적이다. 
- Image & 컨테이너 관리 & Pod 생성이 가능해야 한다. 
- CRI 를 지원하는 컨테이너 런타임은 containerd, CRI-O 가 대표적이다. 

Docker 의 경우, dockerd가 CRI 를 구현하지 않았기 떄문에, dockerd-shim이 필수적이었다. 
하지만 이제 Docker 에서도 **containerd**를 기본으로 사용한다. 
따라서 K8S 를 사용할때, 도커가 필수적이지 않다. 

---

Client => dockerd => containerd => runc

**Dockerd**

- REST API 를 제공, client 로 부터 직접 컨테이너 관련 요청을 받는다. 
- 대부분은 containerd 로 위임하지만 일부 작업 (이미지 빌드, 네트워킹) 은 직접 수행. 
- 기본값은 unix domain socket 이지만, Tcp 방식도 가능 

**Podman**

- 데몬을 사용하는 방식이 아닌, directly using runC.
- Podman 직접적으로 image registry, containers, image storage 접근

- 쿠버네티스와 도커   <img src="https://www.itopstimes.com/wp-content/uploads/2018/05/docker-ce.png" width="250" algin="left"/>  <img src="https://storage.googleapis.com/static.ianlewis.org/prod/img/771/docker.png" width="250" algin="left"/>

- 도커와 Podman 간의 비교   <img src="https://miro.medium.com/max/1400/1*OPQDWqLLXCUZSfq8oiCaug.png" width="250" algin="left"/><img src="https://miro.medium.com/max/1400/1*EN7d_9nCJEfp_mP7erY_7w.png" width="250" algin="left"/>

https://medium.com/technopanti/docker-vs-podman-c03359fabf77



- 컨테이너 런타임의 변화 과정   <img src="https://i1.wp.com/www.opennaru.com/wp-content/uploads/2019/07/Containerd_version.png?fit=1482%2C837" width="600" algin="left"/>





### 
