using System;
using System.IO;
using 
namespace ScriptEngine {
    class Script {
        public Script() {
            Console.WriteLine("Hello world");
        }
        public int GetInt(String value) {
            Console.WriteLine("You gave: " + value);
            return 10;
        }
    }
}
