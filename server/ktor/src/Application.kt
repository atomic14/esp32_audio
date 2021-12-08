package atomic14.esp32.audio

import io.ktor.application.call
import io.ktor.application.install
import io.ktor.features.CallLogging
import io.ktor.features.DefaultHeaders
import io.ktor.html.respondHtml
import io.ktor.http.ContentType
import io.ktor.http.HttpStatusCode
import io.ktor.http.content.files
import io.ktor.http.content.staticRootFolder
import io.ktor.http.fromFilePath
import io.ktor.request.path
import io.ktor.request.receive
import io.ktor.response.respondRedirect
import io.ktor.routing.Route
import io.ktor.routing.get
import io.ktor.routing.post
import io.ktor.routing.route
import io.ktor.routing.routing
import io.ktor.server.engine.embeddedServer
import io.ktor.server.netty.Netty
import io.ktor.util.combineSafe
import java.io.File
import java.io.InputStream
import java.nio.file.Files
import java.nio.file.StandardOpenOption
import java.text.SimpleDateFormat
import java.util.*
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.withContext
import kotlinx.html.a
import kotlinx.html.body
import kotlinx.html.h1
import kotlinx.html.hr
import kotlinx.html.style
import kotlinx.html.table
import kotlinx.html.tbody
import kotlinx.html.td
import kotlinx.html.th
import kotlinx.html.thead
import kotlinx.html.tr

fun main() {
    val root = File("files").takeIf { it.exists() } ?: error("Can't locate files folder")

    embeddedServer(Netty, port = 5003) {
        install(DefaultHeaders)
        install(CallLogging)
        routing {
            get("/") {
                println("get my files")
                call.respondRedirect("/myfiles")
            }
            post("/adc_samples") {
                println("post adc_samples")
                val file = File("files/adc.raw")
                if (file.exists().not()) {
                    file.createNewFile()
                }

                withContext (Dispatchers.IO) {
                    val data = call.receive<InputStream>()
                    Files.write(file.toPath(), data.readBytes(), StandardOpenOption.APPEND)
                }

                call.response.status(HttpStatusCode.OK)
            }
            post("/i2s_samples") {
                println("post i2s_samples")
                val file = File("files", "i2s.raw")
                if (file.exists().not()) {
                    file.createNewFile()
                }

                withContext (Dispatchers.IO) {
                    val data = call.receive<InputStream>()
                    Files.write(file.toPath(), data.readBytes(), StandardOpenOption.APPEND)
                }
                call.response.status(HttpStatusCode.OK)
            }

            route("/myfiles") {
                println("route myfiles")
                files(root)
                listing(root)
            }
        }
    }.start(wait = true)
}

fun Route.listing(folder: File) {
    val dir = staticRootFolder.combine(folder)
    val pathParameterName = "static-content-path-parameter"
    val dateFormat = SimpleDateFormat("dd-MMM-YYYY HH:mm")
    get("{$pathParameterName...}") {
        val relativePath = call.parameters.getAll(pathParameterName)?.joinToString(File.separator) ?: return@get
        val file = dir.combineSafe(relativePath)
        if (file.isDirectory) {
            val isRoot = relativePath.trim('/').isEmpty()
            val files = file.listSuspend(includeParent = !isRoot)
            val base = call.request.path().trimEnd('/')
            call.respondHtml {
                body {
                    h1 {
                        +"Index of $base/"
                    }
                    hr {}
                    table {
                        style = "width: 100%;"
                        thead {
                            tr {
                                for (column in listOf("Name", "Last Modified", "Size", "MimeType")) {
                                    th {
                                        style = "width: 25%; text-align: left;"
                                        +column
                                    }
                                }
                            }
                        }
                        tbody {
                            for (finfo in files) {
                                val rname = if (finfo.directory) "${finfo.name}/" else finfo.name
                                tr {
                                    td {
                                        if (finfo.name == "..") {
                                            a(File(base).parent) { +rname }
                                        } else {
                                            a("$base/$rname") { +rname }
                                        }
                                    }
                                    td {
                                        +dateFormat.format(finfo.date)
                                    }
                                    td {
                                        +(if (finfo.directory) "-" else "${finfo.size}")
                                    }
                                    td {
                                        +(ContentType.fromFilePath(finfo.name).firstOrNull()?.toString() ?: "-")
                                    }
                                }
                            }
                        }
                    }
                    hr {}
                }
            }
        }
    }
}

private fun File?.combine(file: File) = when {
    this == null -> file
    else -> resolve(file)
}

data class FileInfo(val name: String, val date: Date, val directory: Boolean, val size: Long)

suspend fun File.listSuspend(includeParent: Boolean = false): List<FileInfo> {
    val file = this
    return withContext(Dispatchers.IO) {
        listOfNotNull(
            if (includeParent) {
                FileInfo("..", Date(), true, 0L)
            } else {
                null
            }
        ) + file.listFiles().toList().map {
            FileInfo(it.name, Date(it.lastModified()), it.isDirectory, it.length())
        }.sortedWith(comparators(
            Comparator { a, b -> -a.directory.compareTo(b.directory) },
            Comparator { a, b -> a.name.compareTo(b.name, ignoreCase = true) }
        ))
    }
}

fun <T> comparators(vararg comparators: Comparator<T>): Comparator<T> {
    return Comparator { l, r ->
        for (comparator in comparators) {
            val result = comparator.compare(l, r)
            if (result != 0) return@Comparator result
        }
        return@Comparator 0
    }
}

operator fun <T> Comparator<T>.plus(other: Comparator<T>): Comparator<T> = comparators(this, other)
