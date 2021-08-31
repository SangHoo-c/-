

3. openstackcli 는 어떻게 nova api 주소를 알아내나요? & 4. nova 의 어떤 API를 호출하여 결과를 받아오나요?
---------------------------------------------------------------------------------------


server list 명령어를 처리하는 과정중,


.. code-block:: python

   # cliff/display.py
   class DisplayCommandBase(command.Command, metaclass=abc.ABCMeta):
    """Command base class for displaying data about a single object.
    """
    def __init__(self, app, app_args, cmd_name=None):
        super(DisplayCommandBase, self).__init__(app, app_args,
                                                 cmd_name=cmd_name)
        self._formatter_plugins = self._load_formatter_plugins()

    ...

    # list server 의 수행부
    # /compute/v2/server.py 의 take_action 을 수행한다.
    def run(self, parsed_args):     # self = {ListServer}<openstackclient.compute.v2.server.ListServer object at 0x103e9a190>
        parsed_args = self._run_before_hooks(parsed_args)
        self.formatter = self._formatter_plugins[parsed_args.formatter].obj
        column_names, data = self.take_action(parsed_args)
        column_names, data = self._run_after_hooks(parsed_args,
                                                   (column_names, data))
        self.produce_output(parsed_args, column_names, data)
        return 0


server.py 의 take_action 메소드
3 가지의 작업을 수행한다.

.. code-block:: python

       def take_action(self, parsed_args):
        compute_client = self.app.client_manager.compute    # 1
        identity_client = self.app.client_manager.identity  # 2
        image_client = self.app.client_manager.image        # 3

3 개의 메서드 모두 clientmanager.py 로 이동해서
ClientCache 객체를 반환한다.


.. code-block:: python

   # osc_lib/clientmanager.py
   class ClientCache(object):
    """Descriptor class for caching created client handles."""

    def __init__(self, factory):
        self.factory = factory
        self._handle = None

    def __get__(self, instance, owner):
        # Tell the ClientManager to login to keystone
        if self._handle is None:
            try:
                self._handle = self.factory(instance)       # ???? => client.py 로 이동
            except AttributeError as err:
                # Make sure the failure propagates. Otherwise, the plugin just
                # quietly isn't there.
                raise exceptions.PluginAttributeError(err) from err
        return self._handle



.. code-block:: python

   def make_client(instance):
    """Returns a compute service client."""

      compute_api = utils.get_client_class(
        API_NAME,
        version.ver_major,
        COMPUTE_API_VERSIONS,
    )
    ...

와 같은 과정을 3가지 메소드 모두 공통적으로 거친다.

해당 메소드를 실행하고 나면

.. code-block::

   compute_client = {Client} <novaclient.v2.client.Client object at 0x103ec9b50>
   image_client = {Proxy}  <openstack.image.v2._proxy.Proxy object at 0x103eb9910>
   identity_client = {Client} <keystoneclient.v3.client.Client object at 0x103e15c40>

값을 갖는다.


우선 compute client 의 정보를 확인해보자.

.. code-block::

   compute_client = {Client} <novaclient.v2.client.Client object at 0x10302aca0>
     agents = {AgentsManager} <novaclient.v2.agents.AgentsManager object at 0x10302a7f0>
     aggregates = {AggregateManager} <novaclient.v2.aggregates.AggregateManager object at 0x10302ac40>
     api = {APIv2} <openstackclient.api.compute_v2.APIv2 object at 0x10302ae50>
      HEADER_NAME = {str} 'OpenStack-API-Version'
      SERVICE_TYPE = {str} ''
      endpoint = {str} 'http://211.37.148.101/compute/v2.1'

compute client 로 접근하기 위한 endpoint 를 알 수 있다.
**endpoint = {str} 'http://211.37.148.101/compute/v2.1'**


생성한 인스턴스를 이용해 servers.list 메소드를 호출한다.

.. code-block:: python

   # compute/v2/server.py
           data = compute_client.servers.list(search_opts=search_opts,
                                           marker=marker_id,
                                           limit=parsed_args.limit)

드디어, list 메소드는 servers.py 안에 구현되어 있다.

.. code-block:: python


       # novaclient/v2/servers.py
       def list(self, detailed=True, search_opts=None, marker=None, limit=None,
             sort_keys=None, sort_dirs=None):
        """
        Get a list of servers.

        """
        ...

        detail = ""
        if detailed:
            detail = "/detail"

        result = base.ListWithMeta([], None)
        while True:
            ...
            # _list 메소드를 통해 "demo-instance" 를 불러오는 과정
            servers = self._list("/servers%s%s" % (detail, query_string),
                                 "servers")
            result.extend(servers)
            result.append_request_ids(servers.request_ids)
            ...
        return result


self._list("/servers%s%s" % (detail, query_string),"servers") 를 통해 instance 를 불러온다.

url = {str} '/servers/detail'
response_key = {str} 'servers'

.. code-block:: python

   # novaclient/base.py
   def _list(self, url, response_key, obj_class=None, body=None, filters=None):
      if filters:
         url = utils.get_url_with_filter(url, filters)
      if body:
         resp, body = self.api.client.post(url, body=body)
      else:
         resp, body = self.api.client.get(url)      # get 방식으로 호출하는 것을 알 수 있다.



self.api.client.get(url) 는 keystoneauth1/adapter.py 를 호출한다.

.. code-block:: python

   # keystoneauth1/adapter.py
        def get(self, url, **kwargs):
        return self.request(url, 'GET', **kwargs)

resp = 200
body = instance 정보

가 반환된다.



결론
""""""""""""""""""""""""""""""
=> compute_client 객체를 만든다.

=> novaclient/v2/servers.py 의 list 메소드 실행 (url 전달)

=> novaclient/base.py 의 _list 메소드 실행

=> keystoneauth1/adapter.py 에서 get 방식 호출


**http://211.37.148.101/compute/v2.1/servers/detail** 을 통해 호출한다.



5. 결과를 이쁘게 table 형식으로 출력해주는 함수는 무엇일까요?
--------------------------------------------------------


.. code-block:: python

   # cliff/display.py
       def run(self, parsed_args):
        parsed_args = self._run_before_hooks(parsed_args)
        self.formatter = self._formatter_plugins[parsed_args.formatter].obj
        column_names, data = self.take_action(parsed_args)
        column_names, data = self._run_after_hooks(parsed_args,
                                                   (column_names, data))
        self.produce_output(parsed_args, column_names, data)        # ****
        return 0


self = {ListServer} <openstackclient.compute.v2.server.ListServer object at 0x1053288e0>

.. code-block:: python

   # cliff/lister.py
   def produce_output(self, parsed_args, column_names, data):

        ...
        columns_to_include, selector = self._generate_columns_and_selector(
            parsed_args, column_names,
        )

data = {tuple} ('ID', 'Name', 'Status', 'Networks', 'Image', 'Flavor')
이와 같이 column 값을 확인 할 수 있었다.

parsed_args 와 column_names, data 를 인자로 전달받아
_generate_columns_and_selector 메소드를 호출한다.


.. code-block:: python

   # cliff/display.py
       def _generate_columns_and_selector(self, parsed_args, column_names):
        """Generate included columns and selector according to parsed args.

        :param parsed_args: argparse.Namespace instance with argument values
        :param column_names: sequence of strings containing names
                             of output columns
        """
이 메소드를 통해  ('ID', 'Name', 'Status', 'Networks', 'Image', 'Flavor') 이 반환되어
table 의 column 을 알 수 있다.

반환 받고 다시 lister.py 로 돌아간다.

.. code-block:: python

   # cliff/lister.py
   def produce_output(self, parsed_args, column_names, data):
    ...
        self.formatter.emit_list(
            columns_to_include, data, self.app.stdout, parsed_args,
        )


emit_list 메소드를 통해 cliff/table.py  로 이동한다.

.. code-block:: python

   # cliff/table.py
   class TableFormatter(base.ListFormatter, base.SingleFormatter):
   ...

       def emit_list(self, column_names, data, stdout, parsed_args):
        x = prettytable.PrettyTable(
            column_names,
            print_empty=parsed_args.print_empty,
        )
        x.padding_width = 1



table 을 만들때 PrettyTable 클래스의 인스턴스를 생성한다.

.. code-block:: python

   # prettytable/prettytable.py
   class PrettyTable:
    def __init__(self, field_names=None, **kwargs):
    ...


결론
""""""""""""""""""""""""""""""
=> cliff/display.py 에서 produce_output 메소드 호출

=> cliff/lister.py 에서 _generate_columns_and_selector 호출 : columns_to_include, selector 정의

=> cliff/lister.py 에서 emit_list 호출

=> cliff/table.py 에서 prettytable.PrettyTable 호출



