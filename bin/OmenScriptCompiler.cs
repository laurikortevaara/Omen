using System;
using System.IO;
using System.Reflection;
using System.CodeDom.Compiler;
using Microsoft.CSharp;

namespace ScriptEngine
{
  public class ScriptCompiler
  {
      ScriptCompiler(){}

      public Assembly CompileCS(String scriptFile)
      {
          Console.WriteLine("Compiling C# script: " + scriptFile);

          CodeDomProvider codeProvider = new CSharpCodeProvider();
          CompilerParameters compilerParameters = new CompilerParameters();
          compilerParameters.GenerateExecutable = false;
          compilerParameters.GenerateInMemory = true;
          compilerParameters.IncludeDebugInformation = false;
          compilerParameters.WarningLevel = 3;
          compilerParameters.TreatWarningsAsErrors = false;

         /*
          Assembly executingAssembly = Assembly.GetExecutingAssembly();
          compilerParameters.ReferencedAssemblies.Add(executingAssembly.Location);
          compilerParameters.ReferencedAssemblies.Add("./mono/lib/mono/4.0-api/System.dll");
          compilerParameters.ReferencedAssemblies.Add("./mono/lib/mono/4.0-api/System.Windows.Forms.dll");
          compilerParameters.ReferencedAssemblies.Add("./mono/lib/mono/4.0-api/System.Xml.dll");
          compilerParameters.ReferencedAssemblies.Add("./mono/lib/mono/4.0-api/System.Xml.Linq.dll");
          compilerParameters.ReferencedAssemblies.Add("./mono/lib/mono/4.0-api/System.Web.dll");
          compilerParameters.ReferencedAssemblies.Add("./mono/lib/mono/4.0-api/System.Core.dll");
          compilerParameters.ReferencedAssemblies.Add("./mono/lib/mono/4.0-api/System.Data.dll");
          compilerParameters.ReferencedAssemblies.Add("./mono/lib/mono/4.0-api/System.Drawing.dll");
          compilerParameters.ReferencedAssemblies.Add("./mono/lib/mono/4.0-api/Microsoft.CSharp.dll");
          compilerParameters.ReferencedAssemblies.Add("./mono/lib/gtk-sharp-2.0/gtk-sharp.dll");
          compilerParameters.ReferencedAssemblies.Add("./mono/lib/gtk-sharp-2.0/atk-sharp.dll");
          compilerParameters.ReferencedAssemblies.Add("./mono/lib/gtk-sharp-2.0/gdk-sharp.dll");
          compilerParameters.ReferencedAssemblies.Add("./mono/lib/gtk-sharp-2.0/glade-sharp.dll");
          compilerParameters.ReferencedAssemblies.Add("./mono/lib/gtk-sharp-2.0/glib-sharp.dll");
          compilerParameters.ReferencedAssemblies.Add("./mono/lib/gtk-sharp-2.0/gtk-dotnet.dll");
          compilerParameters.ReferencedAssemblies.Add("./mono/lib/gtk-sharp-2.0/pango-sharp.dll");

          foreach (AssemblyName assemblyName in executingAssembly.GetReferencedAssemblies()) {
              Console.WriteLine(@"Writing the ref assembly: " + assemblyName);
              //compilerParameters.ReferencedAssemblies.Add(Assembly.Load(assemblyName).Location);
          }
          */
          string assemblyCode = File.ReadAllText(scriptFile);

          Console.WriteLine("The source script: " + assemblyCode);
          CompilerResults compileResults = codeProvider.CompileAssemblyFromSource(compilerParameters, new string[] { assemblyCode });
          Console.WriteLine("Compiler Results: ");

          foreach ( Object obj in compileResults.Errors )
            Console.WriteLine( "Error:   {0}", obj );

          foreach ( Object obj in compileResults.Output )
            Console.WriteLine( "Output:   {0}", obj );
          // Check the compiler results for errors
          StringWriter sw = new StringWriter();
          foreach (CompilerError ce in compileResults.Errors)
          {
              if (ce.IsWarning) continue;
               sw.WriteLine("{0}({1},{2}: error {3}: {4}", ce.FileName, ce.Line, ce.Column, ce.ErrorNumber, ce.ErrorText);
          }
          // If there were errors, raise an exception...
          string errorText = sw.ToString();
          if (errorText.Length > 0)
            throw new ApplicationException(errorText);

          Assembly CompiledAssembly = compileResults.CompiledAssembly;
          Console.WriteLine("3");
          return CompiledAssembly;
    }
  }
}
