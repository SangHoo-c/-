OpenStack 팀 2주차 : openstack server list 명령어 동작 원리 파악
==========================================================

2주차 2번째 과제에 대한 정리 내용입니다.

1. 인자로 입력받은 server list 를 어떻게 구별해내는가? < 필수 >
2. server list  라는 명령어를 처리하는 파일은 무엇인가? <필수 >
3. openstackcli 는 어떻게 nova api 주소를 알아내나요?
4. nova 의 어떤 API를 호출하여 결과를 받아오나요? ( 어떤 URI 를 호출하나요? )  < 필수 >
5. 결과를 이쁘게 table 형식으로 출력해주는 함수는 무엇일까요?

0. 상속관계
-------------------------------------------------------------
openstackclient  -> osc_lib  -> clif

OpenStackShell -> OpenStackShell ->  App

1. 인자로 입력받은 server list 를 어떻게 구별해내는가? & 2. server list  라는 명령어를 처리하는 파일은 무엇인가?
-------------------------------------------------------------------------------------------------------------------

1. openstackclient/shell.py

.. code-block:: python

   def main(argv=None):
    if argv is None:
        argv = sys.argv[1:]

    return OpenStackShell().run(argv)


a. main 함수의 OpenStackShell().run(argv) 통해 OpenStackShell object 가 생성된다.
b. 해당 객체의 run() 메소드를 실행한다.
argv = ['server', 'list'] 가 run 메소드의 인자로 넘겨진다.

.. code-block:: python

   # openstackclient/shell.py
   # OpneStackShell 객체 선언부
   # 부모 클래스인 osc_lib/shell.OpenStackShell 를 상속한다.
   class OpenStackShell(shell.OpenStackShell):

    def __init__(self):

        super(OpenStackShell, self).__init__(
            description=__doc__.strip(),
            version=openstackclient.__version__,
            command_manager=commandmanager.CommandManager('openstack.cli'), # command manager 를 조금 더 들어가보자.
            deferred_help=True)



.. code-block:: python

   # cliff/commandmanager.py
       def __init__(self, namespace, convert_underscores=True):
        self.commands = {}
        self._legacy = {}
        self.namespace = namespace
        self.convert_underscores = convert_underscores
        self.group_list = []
        self._load_commands()   # load_commands 를 통해

       def load_commands(self, namespace):
        """Load all the commands from an entrypoint"""
        self.group_list.append(namespace)
        for ep in stevedore.ExtensionManager(namespace):
            LOG.debug('found command %r', ep.name)
            cmd_name = (ep.name.replace('_', ' ')
                        if self.convert_underscores
                        else ep.name)
            self.commands[cmd_name] = ep.entry_point
        return

이와 같이 객체 선언을 마친후, run command 를 실행한다.
상속관계에 따라 run 은 clif 의 run 메소드가 실행된다.

.. code-block:: python

   # clif/app.py
           try:
            self.options, remainder = self.parser.parse_known_args(argv)    # remainder = ['server', 'list']
            self.configure_logging()
            self.interactive_mode = not remainder
            if self.deferred_help and self.options.deferred_help and remainder:
                self.options.deferred_help = False
                remainder.insert(0, "help")
            self.initialize_app(remainder)

run 실행 중 initialize_app 라는 메소드가 있다.
remainder =  ['server', 'list'] 를 인자로 받는다.

.. code-block:: python

   # openstackclient/shell.py
       def initialize_app(self, argv):
        super(OpenStackShell, self).initialize_app(argv)


부모 클래스인 osc_lib/shell.py 로 이동해보자.

.. code-block:: python

   # osc_lib/shell.py
       def initialize_app(self, argv):
        """Global app init bits:

        * set up API versions
        * validate authentication info
        * authenticate against Identity if requested
        """

        self._load_plugins()

        self._load_commands()

객체를 초기화 하는 내용이다.
API version 에 대한 내용, authentication info 등이 정의가 된다고 한다.
load_plugin 메소드로 들어가보자.

.. code-block:: python

   # openstackclient/shell.py
   def _load_plugins(self):
       ...
       cmd_group = 'openstack.' + api.replace('-', '_') + version  # cmd_group: 'openstack.compute.v2'
       self.command_manager.add_command_group(cmd_group)


cmd_group 이 선언되었다. 이를 더 자세하게 알아보자.


.. code-block:: python

    # cliff/commandmanager.py

        def add_command_group(self, group=None):    # group = 'openstack.compute.v2'
        """Adds another group of command entrypoints"""
        if group:
            self.load_commands(group)


        def load_commands(self, namespace):     # namespace = 'openstack.compute.v2'
        """Load all the commands from an entrypoint"""
        self.group_list.append(namespace)
        for ep in stevedore.ExtensionManager(namespace):
            LOG.debug('found command %r', ep.name)
            cmd_name = (ep.name.replace('_', ' ')
                        if self.convert_underscores
                        else ep.name)
            self.commands[cmd_name] = ep.entry_point
        return

self = <osc_lib.command.commandmanager.CommandManager object at 0x10a292a90>
이와 같고 이와 같은 명령어들이 commands 안에 들어있음을 볼 수 있다.

.. code-block::

    commands = {dict: 37} {'command list': EntryPoint(name='command_list', value='openstackclient.comm ...
     'command list' = {EntryPoint: 3} EntryPoint(name='command_list', value='openstackclient.common.module:ListCommand', group='openstack.cli')
     'module list' = {EntryPoint: 3} EntryPoint(name='module_list', value='openstackclient.common.module:ListModule', group='openstack.cli')
     'help' = {EntryPointWrapper} <cliff.commandmanager.EntryPointWrapper object at 0x10ace4b20>
     'complete' = {EntryPointWrapper} <cliff.commandmanager.EntryPointWrapper object at 0x10ace4970>
     'aggregate add host' = {EntryPoint: 3} EntryPoint(name='aggregate_add_host', value='openstackclient.compute.v2 ...
     ...


정리하자면, clientmanager => commandmanager => stevedore 흐름으로
어딘가에 저장된 정보를 가져오는 구조라고 할 수 있다.

_load_commands() 도 같은 구조이다.


+) stevedore 란 무엇인가
""""""""""""""""""""""""""""""
여러개의 plug-in 을 동적으로 로딩하게 해주는 라이브러리.
애플리케이션을 실행중에 라이브러리를 로딩하고 싶을때 사용.
여기선 openstack 에서 명령어를 사용하기 위해 모듈을 로딩할때 사용.


self.initialize_app(remainder) 을 통해 필요한 모듈,
entrypoint 로 부터 받은 command 들이
key: "server list", value: serverlist 으로 OpenStackShell object 에 업로드 된다.


.. code-block:: python

    # cliff/app.py
       def run(self, argv):
        """Equivalent to the main program for the application.

        :param argv: input arguments and options
        :paramtype argv: list of str
        """
        try:
            self.options, remainder = self.parser.parse_known_args(argv)
            self.configure_logging()
            self.interactive_mode = not remainder
            if self.deferred_help and self.options.deferred_help and remainder:
                self.options.deferred_help = False
                remainder.insert(0, "help")
            self.initialize_app(remainder)  # remainder = ['server', 'list']
            self.print_help_if_requested()

        result = 1
        if self.interactive_mode:
            result = self.interact()
        else:
            try:
                result = self.run_subcommand(remainder)     # remainder = ['server', 'list']
            except KeyboardInterrupt:
                return _SIGINT_EXIT
        return result

결론적으로 파싱했던 remainder 를 통해서 run_subcommand(remainder) 를 호출한다.

.. code-block:: python

   # cliff/app.py
       def run_subcommand(self, argv):
        try:
            subcommand = self.command_manager.find_command(argv)

   # osc_lib/shell.py
   def find_command(self, argv):
            if name in self.commands:
                found = name  # name = 'server list'
            ...
            if found:   # name 을 key 로 ep 를 가져온다.
                cmd_ep = self.commands[found]
                if hasattr(cmd_ep, 'resolve'):
                    cmd_factory = cmd_ep.resolve()
                else:
                    # NOTE(dhellmann): Some fake classes don't take
                    # require as an argument. Yay?
                    arg_spec = inspect.getfullargspec(cmd_ep.load)
                    if 'require' in arg_spec[0]:
                        cmd_factory = cmd_ep.load(require=False)
                    else:
                        cmd_factory = cmd_ep.load()     # stevdore 를 통해 동적으로 class 로딩

여기서 self.command 는 전에 로딩했던 모든 명령어들이 command 변수에 저장되어 있다.
key - value 로 ep 값을 가져온다.

.. code-block::

   'server list' = {EntryPoint: 3} EntryPoint(name='server_list', value='openstackclient.compute.v2.server:ListServer', group='openstack.compute.v2')
     extras = {list: 0} []
     group = {str} 'openstack.compute.v2'
     name = {str} 'server_list'
     pattern = {Pattern} re.compile('(?P<module>[\\w.]+)\\s*(:\\s*(?P<attr>[\\w.]+))?\\s*(?P<extras>\\[.*\\])?\\s*$')
     value = {str} 'openstackclient.compute.v2.server:ListServer'
     0 = {str} 'server_list'
     1 = {str} 'openstackclient.compute.v2.server:ListServer'
     2 = {str} 'openstack.compute.v2'
     __len__ = {int} 3


.. code-block:: python

   # cliff/app.py
   cmd = cmd_factory(self, self.options, **kwargs)  # class 를 cmd 로 인스턴스화 시킨다.
   # cmd = {ListServer} <openstackclient.compute.v2.server.ListServer object at 0x1103ff640>

        try:
            self.prepare_to_run_command(cmd)
            full_name = (cmd_name
                         if self.interactive_mode
                         else ' '.join([self.NAME, cmd_name])
                         )
            cmd_parser = cmd.get_parser(full_name)
            try:
                parsed_args = cmd_parser.parse_args(sub_argv)
            except SystemExit as ex:
                raise cmd2.exceptions.Cmd2ArgparseError from ex
            result = cmd.run(parsed_args)   # run 을 호출한다.




.. code-block:: python

   # osc_lib/command/command.py
   class Command(command.Command, metaclass=CommandMeta):

    def run(self, parsed_args):
        self.log.debug('run(%s)', parsed_args)
        return super(Command, self).run(parsed_args)


.. code-block:: python

   # cliff/display.py
       def run(self, parsed_args):
        parsed_args = self._run_before_hooks(parsed_args)
        self.formatter = self._formatter_plugins[parsed_args.formatter].obj
        column_names, data = self.take_action(parsed_args)      # 수행부
        column_names, data = self._run_after_hooks(parsed_args,
                                                   (column_names, data))
        self.produce_output(parsed_args, column_names, data)
        return 0


self = {List Server} <openstackclient.compute.v2.server.ListServer object at 0x106473370>
즉, 인자로 전달받은 값을 저장해놓은 command list 에서 꺼낸 값이다.
**따라서, List Server 클래스의 take_action 이 수행된다.**

.. note::
    server list 명령어를 처리해주는 파일은 **openstack/python-openstackclient/openstackclient/compute/v2/server.py 이다.**

결론
""""""""""""""""""""""""""""""

=> plug-in 로딩

=> 모듈을 command manager 갖고 있다

=> 인자로 넘어온게 command list 에 있으면 인자에 맞는 EP 를 반환해준다.

=> EP 를 로딩시킨다.

=> 해당 클래스의 take actions 라는 함수를 실행시킨다.

